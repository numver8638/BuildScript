/*
 * Identifier.h
 * - Represents an identifier.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_IDENTIFIER_H
#define BUILDSCRIPT_COMPILER_IDENTIFIER_H

#include <string>
#include <string_view>

#include <BuildScript/Compiler/SourcePosition.h>

namespace BuildScript {
    /**
     * @brief Represents an identifier.
     */
    class Identifier final {
    private:
        SourceRange m_range;
        std::string m_string;

    public:
        Identifier() = default;

        /**
         * @brief Construct new @c Identifier.
         * @param range a range where the identifier located.
         * @param str a string represents identifier.
         */
        Identifier(SourceRange range, std::string str)
            : m_range(range), m_string(std::move(str)) {}

        Identifier(const Identifier&) = default;

        Identifier(Identifier&&) = default;

        Identifier& operator =(const Identifier&) = default;

        Identifier& operator =(Identifier&&) = default;

        bool operator ==(std::string_view right) const { return Equals(right); }

        bool operator !=(std::string_view right) const { return !Equals(right); }

        /**
         * @copybrief IsValid()
         */
        explicit operator bool() const { return IsValid(); }

        /**
         * @brief Test data is valid.
         * @return @c true if valid, otherwise @c false.
         */
        bool IsValid() const { return (bool)m_range; }

        /**
         * @brief Get a range of the identifier.
         * @return a @c SourceRange representing range of the identifier.
         */
        const SourceRange& GetRange() const { return m_range; }

        /**
         * @brief Get a position of the identifier.
         * @return a @c SourcePosition representing position of the identifier.
         */
        const SourcePosition& GetPosition() const { return m_range.Begin; }

        /**
         * @brief Get a string.
         * @return a string representing the identifier.
         */
        const std::string& GetString() const { return m_string; }

        /**
         * @brief
         * @param str
         * @return
         */
        bool Equals(std::string_view str) const {
            return IsValid() && (m_string == str);
        }
    }; // end class Identifier
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_IDENTIFIER_H