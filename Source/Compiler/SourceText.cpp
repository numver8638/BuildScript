/*
 * SourceText.cpp
 * - Represent source code in memory.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/SourceText.h>

#include <BuildScript/Utils/PointerArith.h>

using namespace BuildScript;

SourceText::SourceText(std::string name, const char* buffer, size_t length, Encoding& encoding, unsigned int tabsize)
    : m_name(std::move(name)), m_begin(buffer), m_current(buffer), m_next(nullptr), m_end(buffer + length),
      m_encoding(encoding), m_tabsize(tabsize) {
    NEVER_BE_NULL(buffer);

    m_current += encoding.CheckPreamble(buffer, buffer + length);
    m_next = m_current;

    ConsumeChar();
}

SourceText::SourceText(const SourceText& parent, const SourceRange& range)
    : m_name(parent.m_name), m_begin(parent.m_begin), m_current(parent.m_begin + range.Begin.Cursor), m_next(nullptr),
      m_end(parent.m_begin + range.End.Cursor), m_encoding(parent.m_encoding), m_tabsize(parent.m_tabsize),
      m_line(range.Begin.Line), m_column(range.Begin.Column - 1) {
    m_next = m_current;

    ConsumeChar();
}

void SourceText::ConsumeChar() {
    if (m_current < m_end) {
        // Update line & column
        switch (m_ch) {
            case u'\r':
            case u'\n':
                ++m_line;
                m_column = 0;
                break;

            case u'\t':
                m_column += m_tabsize - (m_column % m_tabsize);
                break;

            default:
                // Do nothing.
                break;
        }

        ++m_column;

        if (m_next < m_end) {
            size_t length;
            m_ch = m_encoding.DecodeChar(m_next, m_end, length);

            m_current = m_next;
            m_next += length;

            // Check for CRLF (\r\n)
            if (m_ch == u'\r' && m_encoding.DecodeChar(m_next, m_end, length) == u'\n') {
                m_next += length;
            }
        }
        else {
            // Reached at the end.
            m_current = m_next;
            m_ch = EndOfFile;
        }
    }
}

SourcePosition SourceText::GetNextPosition(const SourcePosition& pos) const {
    auto cursor = m_begin + pos.Cursor;

    assert(cursor < m_end);

    size_t length;
    auto ch = m_encoding.DecodeChar(cursor, m_end, length);
    auto line = m_line;
    auto column = m_column;
    cursor += length;

    if (ch == u'\r' && m_encoding.DecodeChar(cursor, m_end, length) == u'\n') {
        cursor += length;
    }

    switch (ch) {
        case u'\r':
        case u'\n':
            ++line;
            column = 0;
            break;

        case u'\t':
            column += m_tabsize - (column % m_tabsize);
            break;

        default:
            // do nothing.
            break;
    }

    ++column;

    return {static_cast<size_t>(cursor - m_begin), line, column};

}

std::string SourceText::GetString(const SourcePosition& begin, const SourcePosition& end) const {
    assert(begin.Cursor <= end.Cursor);
    assert((m_begin + end.Cursor) <= m_end);

    size_t _; // placeholder
    return m_encoding.DecodeString(m_begin + begin.Cursor, m_begin + end.Cursor, _);
}