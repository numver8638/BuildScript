/*
 * Config.h
 * - Compiler specific configurations.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_CONFIG_H
#define BUILDSCRIPT_CONFIG_H

// Compiler detection
#if   defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
#else
    #error Unknown/Unsupported compiler.
#endif

// Compiler specific macros
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
    #define EXPORT_API __attribute__((visibility("default")))
    #define PACKED __attribute__((packed))
    #define DEPRECATED __attribute__((deprecated))
#elif defined(COMPILER_MSVC)
    #ifdef ON_BUILD
        #define EXPORT_API __declspec(dllexport)
    #else
        #define EXPORT_API __declspec(dllimport)
    #endif // ON_BUILD
    #define PACKED __declspec(align(1))
    #define DEPRECATED __declspec(deprecated)
#else
    #define EXPORT_API
#endif // defined(COMPILER_*)

#endif // BUILDSCRIPT_CONFIG_H