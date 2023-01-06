set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -v -Wl,--no-fatal-warnings")
cmake_minimum_required(VERSION 3.4.1)

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

add_library(
        test_jni
        SHARED
        hook/test_jni.cpp
)


target_link_libraries(test_jni    #指定目标库中。与 add_library的库名称一定要相同
        ${log-lib}    # 将目标库链接到日志库包含在NDK。
        )


add_library(
        test_inline_hook
        SHARED
        hook/test_inline_hook.cpp
)


target_link_libraries(test_inline_hook    #指定目标库中。与 add_library的库名称一定要相同
        ${log-lib}    # 将目标库链接到日志库包含在NDK。
        )

