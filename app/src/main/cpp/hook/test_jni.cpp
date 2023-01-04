//
// Created by xw0code on 2023-01-03.
//
#include <jni.h>
#include <dlfcn.h>
#include "logger.h"

#define TAG "TestJNI"
extern "C"
JNIEXPORT jint JNICALL
Java_o_w_testinlinehook_TestJni_getValue(JNIEnv *env, jclass clazz) {
    return -1;
}
