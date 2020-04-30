/*
 * Encoding.h
 * - Represent text encoding.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_ENCODING_H
#define BUILDSCRIPT_UTILS_ENCODING_H

#include <cassert>
#include <string>
#include <map>
#include <vector>

#include <BuildScript/Utils/NonCopyable.h>
#include <BuildScript/Utils/NonMovable.h>
#include <BuildScript/Utils/TypeTraits.h>

namespace BuildScript {
    /**
     * @brief Represent text encoding.
     */
    class Encoding : NonCopyable, NonMovable {
    public:
        static constexpr int InvalidEncoding = -2;
        static constexpr int InvalidCharacter = -3;

    private:
        std::string m_name;

    protected:
        /**
         * @brief Construct and register with given name.
         * 
         * This class is auto-registered class.
         * 
         * @param name an encoding name.
         */
        Encoding(const std::string& name);

        /**
         * @brief Convert unicode character to UTF-8 encoded string.
         * @param ch a valid unicode string.
         * @param buf a buffer to store encoded string.
         * @return a length of converted string.
         */
        int ToUTF8(int ch, char* buf) const;

    public:
        virtual ~Encoding() = default;

        /**
         * @brief Decode encoded byte to unicode character.
         * @param buffer a buffer contains encoded bytes.
         * @param length a count of consumed bytes.
         * @return an unicode character.
         */
        virtual int DecodeChar(const char* buffer, const char* end, int &length) const = 0;

        /**
         * @brief Convert encoded string to UTF-8 string.
         * @param buffer a buffer contains encoded bytes.
         * @param from the first cursor to convert.
         * @param to the end cursor to convert.
         * @param[out] out a converted string. This may be empty or contain partial string when method returns false.
         * @return true if succeed to convert, otherwise false.
         */
        virtual bool Convert(const char* buffer, size_t from, size_t to, std::string &out) const;

        /**
         * @brief Get the name of encoding.
         * @return the name of encoding.
         */
        const std::string& GetName() const { return m_name; }

        /**
         * @brief Get an encoding for given name.
         * @param name the name of encoding.
         * @return a pointer of Encoding if encoding is exist, otherwise nullptr.
         */
        static Encoding* GetEncoding(const std::string& name);

        /**
         * @brief Get an encoding for UTF-8.
         * @return an encoding for UTF-8.
         */
        static Encoding& UTF8() {
            auto encoding = GetEncoding(u8"utf8");

            assert(encoding != nullptr);

            return *encoding;
        }

        /**
         * @brief Get an encoding for UTF-16.
         * @return an encoding for UTF-16.
         */
        static Encoding& UTF16() {
            auto encoding = GetEncoding(u8"utf16");

            assert(encoding != nullptr);

            return *encoding;
        }

        /**
         * @brief Get an encoding for UTF-16 BE.
         * @return an encoding for UTF-16 BE.
         */
        static Encoding& UTF16BE() {
            auto encoding = GetEncoding(u8"utf16be");

            assert(encoding != nullptr);

            return *encoding;
        }

        /**
         * @brief Get an encoding for UTF-32.
         * @return an encoding for UTF-32.
         */
        static Encoding& UTF32() {
            auto encoding = GetEncoding(u8"utf32");

            assert(encoding != nullptr);

            return *encoding;
        }

        /**
         * @brief Get an encoding for UTF-32 BE.
         * @return an encoding for UTF-32 BE.
         */
        static Encoding& UTF32BE() {
            auto encoding = GetEncoding(u8"utf32be");

            assert(encoding != nullptr);

            return *encoding;
        }

        /**
         * @brief Get a list of supported encodings.
         * @return a list of supported encodings.
         */
        const std::vector<Encoding*> GetEncodings();
    }; // end class Encoding
} // end namespace BuildScript

#endif // BUILDSCRIPT_UTILS_ENCODING_H