/*
 * Parser.Shared.h
 * - Shared code for Parser.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_PARSER_SHARED_H
#define BUILDSCRIPT_COMPILER_PARSER_SHARED_H

#include <cassert>
#include <tuple>
#include <utility> // for std::forward
#include <vector>

#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/ParseResult.h>
#include <BuildScript/Compiler/SourceText.h>
#include <BuildScript/Utils/TypeTraits.h>

template <typename T>
inline bool MakeStatus(BuildScript::ParseResult<T>& result) {
    return result.HasError();
}

inline bool MakeStatus(bool b) {
    return b;
}

inline bool MakeStatus(BuildScript::SourcePosition& pos) {
    return pos.IsValid();
}

inline bool MakeStatus(BuildScript::StringRef& ref) {
    return ref.IsValid();
}

template <typename T, typename... Args>
inline bool MakeStatus(T&& t, Args&&... args) {
    return MakeStatus(std::forward<T>(t)) || MakeStatus(std::forward<Args>(args)...);
}

template <typename T> class ReferencedParseResult;

template <typename T>
ReferencedParseResult<T> Result(bool &error, T* (&value));

template <typename T>
class ReferencedParseResult {
    friend ReferencedParseResult Result<T>(bool &, T* (&));

private:
    bool& m_error;
    T* (&m_value);

    ReferencedParseResult(bool& error, T* (&value))
        : m_error(error), m_value(value) {}

public:
    template <typename U, typename = is_base_of_t<T, U>>
    ReferencedParseResult& operator =(const BuildScript::ParseResult<U>& result) {
        m_error |= result.HasError();
        m_value = result.GetValue();
        return *this;
    }
};

template <typename T>
ReferencedParseResult<T> Result(bool &error, T* (&value)) {
    return ReferencedParseResult<T>(error, value);
}

#endif // BUILDSCRIPT_COMPILER_PARSER_SHARED_H