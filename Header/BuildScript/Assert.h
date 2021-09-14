/*
 * Assert.h
 * - Assertions.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_ASSERT_H
#define BUILDSCRIPT_ASSERT_H

#include <cassert>

#define NOT_REACHABLE       (assert(!"never reached."))
#define NEVER_BE_NULL(e)    (assert((e) != nullptr && "never be null."))
#define NEVER_BE_CALLED     (assert(!"never be called. check the code."))

#endif // BUILDSCRIPT_ASSERT_H