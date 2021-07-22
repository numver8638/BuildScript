/*
 * Encoding.h
 * - Encode or decode character or string.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_UTILS_ENCODING_H
#define BUILDSCRIPT_UTILS_ENCODING_H

#include <string>

#include <BuildScript/Assert.h>
#include <BuildScript/Config.h>
#include <BuildScript/Utils/NonCopyable.h>
#include <BuildScript/Utils/NonMovable.h>

namespace BuildScript {
    /**
     * @brief Encode or decode character or string.
     */
    class EXPORT_API Encoding : NonCopyable, NonMovable {
    public:
        /**
         * @brief Represents not decodable character with given encoding.
         */
        static constexpr char32_t InvalidEncoding = static_cast<char32_t>(-2);

        /**
         * @brief Represents invalid character.
         */
        static constexpr char32_t InvalidCharacter = static_cast<char32_t>(-3);

    private:
        std::string m_name;

    protected:
        /**
         * @brief Construct @c Encoding.
         * @param name the name of the encoding.
         */
        explicit Encoding(std::string name) noexcept
            : m_name(std::move(name)) {}

    public:
        virtual ~Encoding() = default;

        /**
         * @brief Decode one character from buffer.
         * @param buffer the buffer to read.
         * @param end the end of the buffer.
         * @param [out] used count of consumed bytes for decode.
         * @return decoded character.
         */
        virtual char32_t DecodeChar(const void* buffer, const void* end, size_t &used) = 0;

        /**
         * @brief Encode one character to buffer.
         * @param ch the character to encode.
         * @param buffer the buffer to write.
         * @param end the end of the buffer.
         * @param [out] length count of consumed bytes for encode.
         * @return @c true if encoded successfully, otherwise @c false.
         */
        virtual bool EncodeChar(char32_t ch, void* buf, const void* end, size_t &length) = 0;

        /**
         * @brief Decode string from buffer.
         * @param buffer the buffer to read.
         * @param end the end of the buffer.
         * @param [out] used count of consumed bytes for decode.
         * @return decoded string. This may be empty if there was an error or buffer is empty.
         */
        virtual std::string DecodeString(const void* buffer, const void* end, size_t &used);

        /**
         * @brief Encode string to buffer.
         * @param string the string to encode.
         * @param buffer the buffer to write.
         * @param end the end of the buffer.
         * @return count of written bytes.
         *
         * @warning This method assumes that @c string is always valid UTF-8 encoded string.
         *          It may cause undefined behavior if invalid encoded string was given.
         */
        virtual size_t EncodeString(const std::string& string, void* buffer, const void* end);

        /**
         * @brief Get name of the encoding.
         *
         * @return the name of the encoding.
         */
        const std::string& GetName() const { return m_name; }

        /**
         * @brief Check preamble and return length of it.
         * @param buffer
         * @param end
         * @return 0 if there's no preamble nor preamble is not defined, otherwise returns length of the preamble.
         */
        virtual size_t CheckPreamble(const void* buffer, const void* end) { return 0; }

        /**
         * @brief Get @c Encoding by name.
         *
         * @param name the name of the encoding.
         * @return pointer of the @c Encoding if ~~, else @c nullptr.
         */
        static Encoding* GetEncoding(const std::string& name);

        /**
         * @brief Get reference of @c Encoding of UTF-8 encoding.
         * @return reference of @c Encoding of UTF-8 encoding.
         */
        static Encoding& UTF8() {
            auto* encoding = GetEncoding("utf-8");

            NEVER_BE_NULL(encoding);

            return *encoding;
        }

        /**
         * @brief Register encoding.
         * @param encoding itself.
         *
         * @warning This method is intended to access from internal class. Do not access directly.
         */
        static void Register(Encoding* encoding);
    }; // end class Encoding
} // end class BuildScript

#endif // BUILDSCRIPT_UTILS_ENCODING_H