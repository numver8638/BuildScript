/*
 * Version.h
 * - Get BuildScript's version information.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_VERSION_H
#define BUILDSCRIPT_VERSION_H

#include <BuildScript/Utils/AllStatic.h>

namespace BuildScript {
    /**
     * @brief .
     */
    class EXPORT_API Version : AllStatic {
    public:
        /**
         * @brief Get major version number.
         * @return Version number.
         */
        static int GetVersionMajor();

        /**
         * @brief Get minor version number.
         * @return Version number.
         */
        static int GetVersionMinor();

        /**
         * @brief Get patch version number.
         * @return Version number.
         */
        static int GetVersionPatch();

        /**
         * @brief Get full version string.
         * @return Version string.
         */
        static const char* const GetVersion();

        /**
         * @brief Get host OS type.
         * @return Current host OS type. One of "Windows", "MacOS", "Linux".
         * @todo Add other OSes.
         */
        static const char* const GetOSType();

        /**
         * @brief Get host architecture type.
         * @return Current host architecture type. One of "x86", "x64", "ARM", "AArch64".
         * @todo Add other architectures.
         */
        static const char* const GetArchType();
    }; // end class Version
} // end namespace BuildScript

#endif // BUILDSCRIPT_VERSION_H