//
// Created by xw0code on 2023-01-03.
//

#ifndef TESTINLINEHOOK_LOGGER_H
#define TESTINLINEHOOK_LOGGER_H
#endif //TESTINLINEHOOK_LOGGER_H
#include <android/log.h>

// 定义info信息
#define LOGI(TAG,...)     __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(TAG,...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
