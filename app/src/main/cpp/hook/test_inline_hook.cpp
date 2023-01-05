//
// Created by xw0code on 2023-01-03.
//
#include <jni.h>
#include <dlfcn.h>
#include "logger.h"
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <csignal>
#include <sys/mman.h>
// #include <asm/ptrace.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include "relocate.h"

#define TAG "InlineHook"


enum hook_status {
    REGISTERED,
    HOOKED,
};


struct InlineHookItem {
    uint32_t target_addr;
    uint32_t new_addr;
    int length;
    int status;
    void *orig_instructions;
    void *trampoline_instructions;
};

struct inlineHookInfo {
    struct InlineHookItem items[1024];
    int size;
};

static struct inlineHookInfo info = {0};

void *(*oriGetValue)(JNIEnv *env, jclass clazz);

void *newGetValue(JNIEnv *env, jclass clazz) {
    LOGI(TAG, "新的函数");
    jint o_ret = (jint) oriGetValue(env, clazz);
    jclass testJNIClazz = env->FindClass("o/w/testinlinehook/TestJni");
    jmethodID realValueMethodID = env->GetStaticMethodID(testJNIClazz, "realValue", "()I");
    jint realValue = env->CallStaticIntMethod(testJNIClazz, realValueMethodID);
    return reinterpret_cast<void *>(realValue);
}

static struct InlineHookItem *findInlineHookItem(uint32_t target_addr) {
    int i;
    for (i = 0; i < info.size; ++i) {
        if (info.items[i].target_addr == target_addr) {
            return &info.items[i];
        }
    }
    return nullptr;
}


static struct InlineHookItem *addInlineHookItem() {
    struct InlineHookItem *item;

    if (info.size >= 1024) {
        return nullptr;
    }
    item = &info.items[info.size];
    ++info.size;

    return item;
}

void removeInlineHookItem(int pos) {
    info.items[pos] = InlineHookItem{};
    --info.size;
}

extern "C"
JNIEXPORT void JNICALL
Java_o_w_testinlinehook_TestInlineHook_inlineHook(JNIEnv *env, jclass clazz) {
    void *libtest_jni_addr = dlopen("libtest_jni.so", RTLD_NOW);
    void *get_value_addr = dlsym(libtest_jni_addr, "Java_o_w_testinlinehook_TestJni_getValue");
    if (get_value_addr != nullptr) {
        LOGI(TAG, "获取getValue地址成功,addr:%d", (int) get_value_addr);
        InlineHookItem *item = findInlineHookItem((uint32_t) get_value_addr);
        if (item != nullptr) {
            if (item->status == REGISTERED) {
                LOGE(TAG, "已经注册");
                return;
            } else if (item->status == HOOKED) {
                LOGE(TAG, "已经HOOK");
                return;
            } else {
                LOGE(TAG, "未知错误");
                return;
            }
        }


        item = addInlineHookItem();

        item->target_addr = (uint32_t) get_value_addr;
        item->new_addr = (uint32_t) newGetValue;
        item->length = TEST_BIT0(item->target_addr) ? 12 : 8;
        item->orig_instructions = malloc(item->length);
        memcpy(item->orig_instructions, (void *) CLEAR_BIT0(item->target_addr), item->length);
        item->trampoline_instructions = mmap(nullptr, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC,
                                             MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
        //item->proto_addr = (uint32_t **) &newGetValue;
        mprotect((void *) PAGE_START(CLEAR_BIT0(item->target_addr)), PAGE_SIZE * 2,
                 PROT_READ | PROT_WRITE | PROT_EXEC);
        if (TEST_BIT0(item->target_addr)) {
            ((uint16_t *) item->trampoline_instructions)[0] = 0xF8DF;
            ((uint16_t *) item->trampoline_instructions)[1] = 0xF000;
            ((uint16_t *) item->trampoline_instructions)[2] = item->new_addr & 0xFFFF;
            ((uint16_t *) item->trampoline_instructions)[3] = item->new_addr >> 16;
            ((uint16_t *) item->trampoline_instructions)[4] = ((uint16_t *) item->orig_instructions)[0];
            ((uint16_t *) item->trampoline_instructions)[5] = ((uint16_t *) item->orig_instructions)[1];
            ((uint16_t *) item->trampoline_instructions)[6] = ((uint16_t *) item->orig_instructions)[2];
            ((uint16_t *) item->trampoline_instructions)[7] = ((uint16_t *) item->orig_instructions)[3];
            ((uint16_t *) item->trampoline_instructions)[8] = 0xF8DF;
            ((uint16_t *) item->trampoline_instructions)[9] = 0xF000;
            ((uint16_t *) item->trampoline_instructions)[10] =
                    ((item->target_addr - 1) + 9) & 0xFFFF;
            ((uint16_t *) item->trampoline_instructions)[11] = ((item->target_addr - 1) + 9) >> 16;
            auto a = (uint32_t *) &((uint8_t *) item->trampoline_instructions)[9];
            oriGetValue = reinterpret_cast<void *(*)(JNIEnv *, jclass)>(a);
            /*uint32_t a = CLEAR_BIT0(item->target_addr);
            uint32_t b = item->target_addr - 1;
            auto c = &((uint32_t *) item->trampoline_instructions)[0];*/
        }
        if (!TEST_BIT0(item->target_addr)) {
            LOGI(TAG, "HOOK ARM地址");
            ((uint32_t *) (item->target_addr))[0] = 0xe51ff004;// LDR PC, [PC, #-4]
            ((uint32_t *) (item->target_addr))[1] = item->new_addr;
        } else {
            LOGI(TAG, "HOOK THUMB地址");
            int i;

            i = 0;
            if (CLEAR_BIT0(item->target_addr) % 4 != 0) {
                ((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = 0xBF00;  // NOP
            }
            ((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = 0xF8DF;
            ((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = 0xF000;    // LDR.W PC, [PC]
            uint32_t t_addr = ((uint32_t) &((uint8_t *) item->trampoline_instructions)[1]);
            ((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = t_addr & 0xFFFF;
            ((uint16_t *) CLEAR_BIT0(item->target_addr))[i++] = t_addr >> 16;
        }
        /*relocateInstruction(item->target_addr, item->orig_instructions, item->length,
                            item->trampoline_instructions, item->orig_boundaries,
                            item->trampoline_boundaries, &item->count);*/
        item->status = REGISTERED;
    } else {
        LOGE(TAG, "获取getValue地址失败");
    }
}





