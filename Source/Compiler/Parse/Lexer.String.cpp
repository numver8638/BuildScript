/*
 * Lexer.String.cpp
 * - Token scanner for BuildScript language.
 *   This file has specific code for string.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Lexer.h>

#include <vector>
#include <sstream>

#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/SourceText.h>
#include <BuildScript/Utils/CharType.h>

using namespace BuildScript;

std::stringstream& operator <<(std::stringstream& SS, char32_t ch) {
    auto& UTF8 = Encoding::UTF8();
    char buffer[8];
    size_t length;

    if (IsInvalidCharacter(ch) || !UTF8.EncodeChar(ch, buffer, buffer + sizeof(buffer), length)) {
        SS.write("\xEF\xBF\xBD", 3); // U+FFFD with UTF-8 encoding.
    }
    else {
        SS.write(buffer, length);
    }

    return SS;
}

Token Lexer::LexString(SourcePosition begin, char32_t quote) {
    assert(quote == u'\'' || quote == u'"');

    auto isInvalid = false;
    std::stringstream stream;
    char32_t ch;
    uint32_t index = 0;

    while (true) {
        ch = m_source.PeekChar();

        if (ch == quote || IsEOL(ch)) { break; }

        // EOF, EOL, quotes are handled in loop condition.
        switch (ch) {
            case u'$':
                isInvalid |= SkipInterpolation();
                stream << "{" << index++ << "}";
                break;

            case u'\\':
                stream << LexEscape();
                break;

            default:
                stream << ch;
                m_source.ConsumeChar();
                break;
        }
    }

    if (IsEOL(ch)) {
        // If loop breaks because of EOL, report it.
        m_reporter.Report(m_source.GetPosition(), ReportID::LexUnexpectedEOS);
        isInvalid = true;
    }
    else {
        assert(m_source.PeekChar() == quote);
        m_source.ConsumeChar(); // Consume quote
    }

    auto end = m_source.GetPosition();

    return { isInvalid ? TokenType::Invalid : TokenType::String, SourceRange(begin, end), stream.str() };
}

#define TO_UPPER(ch) ((ch) & ~0x20)

char32_t Lexer::LexEscape() {
    assert(m_source.PeekChar() == u'\\');

    m_source.ConsumeChar(); // Consume backslash
    int count;

    switch (m_source.AdvanceChar()) {
        case u'\'': return u'\'';
        case u'\"': return u'\"';
        case u'\\': return u'\\';
        case u'$':  return u'$';
        case u'0':  return u'\0';
        case u'a':  return u'\a';
        case u'b':  return u'\b';
        case u'f':  return u'\f';
        case u'n':  return u'\n';
        case u'r':  return u'\r';
        case u't':  return u'\t';
        case u'v':  return u'\v';

        case u'u':
            count = 4;
            goto HandleHex;

        case u'U':
            count = 8;
            goto HandleHex;

        case u'x':
            count = 2;
            goto HandleHex;

        HandleHex: {
            assert(count > 0);
            char32_t escape = 0;

            char32_t ch;
            while (count-- > 0 && IsHexadecimal((ch = m_source.PeekChar()))) {
                m_source.ConsumeChar();
                escape *= 16;

                if (IsDecimal(ch)) {
                    escape += (ch - u'0');
                }
                else /* 'A' <= ch && ch <= 'F' || 'a' <= ch && ch <= 'f' */ {
                    escape += (TO_UPPER(ch) - u'A');
                }
            }

            if (count > 0) {
                m_reporter.Report(m_source.GetPosition(), ReportID::LexIncompleteEscape);
                escape = SourceText::InvalidCharacter;
            }
            else if (IsInvalidCharacter(ch)) {
                m_reporter.Report(SourcePosition(), ReportID::LexInvalidEscape);
                escape = SourceText::InvalidCharacter;
            }

            return escape;
        }

        default:
            // report error
            m_reporter.Report(m_source.GetPosition(), ReportID::LexInvalidEscape);
            return SourceText::InvalidCharacter;
    }
}

bool Lexer::SkipInterpolation() {
    // Verification only in here. Parsing is done in ScanInterpolations.
    assert(m_source.PeekChar() == u'$');

    auto isInvalid = false;
    m_source.ConsumeChar(); // Consume '$'

    if (!m_source.ConsumeIf(u'{')) {
        if (IsIdentifier(m_source.PeekChar())) {
            m_source.ConsumeWhile(IsIdentifier);
        }
        else {
            m_reporter.Report(m_source.GetPosition(), ReportID::LexInvalidInterpolation);
            isInvalid = true;
        }
    }
    else {
        std::vector<char> delimiters;
        delimiters.push_back('}');

        const auto InString = [&]() -> bool { return delimiters.back() != '}'; };

        while (!delimiters.empty()) {
            auto ch = m_source.PeekChar();

            if (IsEOL(ch)) {
                // Caller will complain this.
                return /*isInvalid=*/true;
            }
            else if (delimiters.back() == ch) {
                delimiters.pop_back();
            }
            else {
                switch (ch) {
                    case u'\'': case u'\"':
                        // Enter inner string.
                        delimiters.push_back(static_cast<char>(ch));
                        break;

                    case u'#':
                        if (!InString()) {
                            // Comment in interpolation is not allowed.
                            m_reporter.Report(m_source.GetPosition(), ReportID::LexCommentInInterpolatedString);
                            isInvalid = true;
                        }
                        break;

                    case u'$':
                        if (!InString()) {
                            // Invalid character - $ is not a identifier.
                            m_reporter.Report(m_source.GetPosition(), ReportID::LexUnknownToken);
                        }
                        else {
                            m_source.ConsumeChar();

                            if (m_source.PeekChar() == u'{') {
                                delimiters.push_back(u'}');
                            }
                            else if (!IsIdentifier(m_source.PeekChar())) {
                                m_reporter.Report(m_source.GetPosition(), ReportID::LexInvalidInterpolation);
                                isInvalid = true;
                            }
                        }
                        break;

                    case u'\\':
                        if (!InString()) {
                            m_reporter.Report(m_source.GetPosition(), ReportID::LexUnknownToken);
                            isInvalid = true;
                        }
                        else {
                            // Skip escape. This will be handled in LexString().
                            m_source.ConsumeChar();
                        }
                        break;

                    case u'{':
                        delimiters.push_back(u'}');
                        break;

                    default:
                        // do nothing.
                        break;
                }
            }

            m_source.ConsumeChar();
        }
    }

    return isInvalid;
}

std::vector<SourceRange> Lexer::ScanInterpolations(const SourceRange& range) {
    SourceText source(m_source, range);

    auto quote = source.AdvanceChar();

    assert(quote == u'\'' || quote == u'"');

    std::vector<SourceRange> ranges;

    char32_t ch;
    while ((ch = source.AdvanceChar()) != quote) {
        if (ch == u'\\') {
            // Skip escape
            source.ConsumeChar();
        }
        else if (ch == u'$') {
            // Interpolated string is already verified in SkipInterpolation().
            // Thus, no error handing in here.
            if (source.ConsumeIf(u'{')) {
                std::vector<char> delimiters;
                delimiters.push_back('}');

                auto open = source.GetPosition();

                while (!delimiters.empty()) {
                    ch = source.PeekChar();

                    if (delimiters.back() == ch) {
                        delimiters.pop_back();

                        if (delimiters.empty()) {
                            // End of interpolation.
                            auto close = source.GetPosition();

                            ranges.emplace_back(open, close);
                        }
                    }
                    else {
                        switch (ch) {
                            case u'\'': case u'\"':
                                // Enter inner string.
                                delimiters.push_back(static_cast<char>(ch));
                                break;

                            case u'$':
                                // Enter nested interpolated expression.
                                source.ConsumeChar();

                                if (source.PeekChar() == u'{') {
                                    delimiters.push_back(u'}');
                                }
                                break;

                            case u'{':
                                // Trace braces to find matching braces.
                                delimiters.push_back(u'}');
                                break;

                            default:
                                // do nothing.
                                break;
                        }
                    }

                    source.ConsumeChar();
                }
            }
            else {
                auto begin = source.GetPosition();
                auto end = source.ConsumeWhile(IsIdentifier);

                ranges.emplace_back(begin, end);
            }
        }
    }

    return std::move(ranges);
}