/*
 * ParseResult.h
 * - Represent result of parser.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_PARSERESULT_H
#define BUILDSCRIPT_COMPILER_PARSERESULT_H

#include <BuildScript/Utils/TypeTraits.h>

namespace BuildScript {
    class Declaration;
    class Statement;
    class Expression;

    /**
     * @brief Represent result of parser.
     */
    template <typename T>
    class ParseResult {
    private:
        bool m_error;
        T* m_value;

    public:
        ParseResult()
            :  m_error(false), m_value(nullptr) {}

        template <typename U, typename = is_base_of_t<T, U>>
        ParseResult(bool error, U* v)
            : m_error(error), m_value(v) {}

        template <typename U, typename = is_base_of_t<T, U>>
        ParseResult& operator =(const ParseResult<U>& rhs) {
            m_error = rhs.m_error;
            m_value = rhs.m_value;

            return *this;
        }

        ParseResult(std::nullptr_t) : ParseResult() {}

        bool HasError() const { return m_error; }

        void Taint() { m_error = true; }

        T* GetValue() const { return m_value; }

        bool HasValue() const { return m_value != nullptr; }

        T* operator ->() const { return GetValue(); }
    };

    using DeclResult = ParseResult<Declaration>;
    using StmtResult = ParseResult<Statement>;
    using ExprResult = ParseResult<Expression>;
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_PARSERESULT_H