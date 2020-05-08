/*
 * StringRef.cpp
 * - A reference of string in the source text.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/StringRef.h>

#include <cassert>

#include <BuildScript/Compiler/SourceText.h>

using namespace BuildScript;

std::string StringRef::ToString() const  {
    assert(IsValid());
    return m_text->GetString(m_begin, m_end);
}