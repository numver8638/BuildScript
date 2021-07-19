/*
 * Version.h
 * - Get BuildScript's version.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_VERSION_H
#define BUILDSCRIPT_VERSION_H

#include <BuildScript/Config.h>
#include <BuildScript/Utils/AllStatic.h>

namespace BuildScript {
    /**
     * @brief Get BuildScript's version.
     */
    class EXPORT_API Version : AllStatic {
    public:
        /**
         * @brief Get a version string.
         * @return a version string.
         */
        const char* GetVersion();

        /**
         * @brief
         * @return a major version number.
         */
        static int GetVersionMajor();

        /**
         * @brief .
         * @return a minor version number.
         */
        static int GetVersionMinor();

        /**
         * @brief .
         * @return a revision version number.
         */
        static int GetVersionRevision();
    }; // end class Version
} // end namespace BuildScript

#endif // BUILDSCRIPT_VERSION_H