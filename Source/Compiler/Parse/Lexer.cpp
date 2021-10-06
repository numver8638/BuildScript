/*
 * Lexer.cpp
 * - Token scanner for BuildScript language.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Lexer.h>

#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/SourceText.h>
#include <BuildScript/Utils/CharType.h>

using namespace BuildScript;

Token Lexer::GetNextToken() {
    while (true) {
        // Skip whitespaces
        auto start = m_source.ConsumeWhile(IsWhitespace);
        auto type = TokenType::Invalid;

        switch (auto ch = m_source.AdvanceChar()) {
            // Error cases
            case SourceText::InvalidEncoding:
                m_reporter.Report(m_source.GetPosition(), ReportID::LexInvalidEncoding, m_source.GetEncodingName());
                continue;

            case SourceText::InvalidCharacter:
                m_reporter.Report(m_source.GetPosition(), ReportID::LexInvalidCharacter);
                continue;

            case SourceText::EndOfFile:
                return { TokenType::EndOfFile, SourceRange(start, m_source.GetPosition()) };

            // Numbers
            case u'0':
                switch (m_source.PeekChar()) {
                    // 0b or 0B
                    case u'b': case u'B':
                        return LexInteger(start, IsBinary, ReportID::LexIncompleteBinary);

                    // 0o or 0O
                    case u'o': case u'O':
                        return LexInteger(start, IsOctal, ReportID::LexIncompleteOctal);

                    // 0x or 0X
                    case u'x': case u'X':
                        return LexInteger(start, IsHexadecimal, ReportID::LexIncompleteHex);

                    default:
                        // Do nothing.
                        break;
                }
                [[fallthrough]];

            case u'1': case u'2': case u'3': case u'4':
            case u'5': case u'6': case u'7': case u'8': case u'9':
                return LexNumber(start);

            // Strings
            case u'\'': case u'\"':
                return LexString(start, ch);

            // Comments
            case u'#':
                return { TokenType::Comment, SourceRange(start, m_source.ConsumeUntil(IsEOL)) };

            default:
                // Check character is in acceptable unicode range.
                if (IsNotAllowedUnicodeInitially(ch)) {
                    m_reporter.Report(start, ReportID::LexNotAllowedCharacter);
                    continue;
                }
                if (!IsAllowedUnicode(ch)) {
                    m_reporter.Report(start, ReportID::LexUnknownToken);
                    continue;
                }
                [[fallthrough]];

            // Identifiers
            // These letters be never first letter of keywords.
            case u'_':
            /*     a*/ /*     b*/ /*     c*/ /*     d*/ /*     e*/ /*     f*/ /*     g*/
            case u'h': /*     i*/ case u'j': case u'k': case u'l': /*     m*/ /*     n*/
            /*     o*/ /*     p*/ case u'q': /*     r*/ /*     s*/ /*     t*/ case u'u':
            /*     v*/ /*     w*/ case u'x': case u'y': case u'z':
            case u'A': case u'B': case u'C': case u'D': case u'E': case u'F': case u'G':
            case u'H': case u'I': case u'J': case u'K': case u'L': case u'M': case u'N':
            case u'O': case u'P': case u'Q': case u'R': case u'S': case u'T': case u'U':
            case u'V': case u'W': case u'X': case u'Y': case u'Z': {
                auto end = m_source.ConsumeWhile(IsIdentifier);
                return { TokenType::Identifier, SourceRange(start, end) };
            }

            // Keywords & Identifiers
            case u'a':  // and, as, assert
            case u'b':  // break
            case u'c':  // case, class, continue, const
            case u'd':  // def, default, defined, deinit, dependsOn, do, doFirst, doLast
            case u'e':  // else, except, export, extends
            case u'f':  // false, finally, for, from
            case u'g':  // get
            case u'i':  // if, import, in, init, inputs, is
            case u'm':  // match
            case u'n':  // none, not
            case u'o':  // operator, or, outputs
            case u'p':  // pass
            case u'r':  // raise, return
            case u's':  // self, set, static, super
            case u't':  // task, true, try
            case u'v':  // var
            case u'w': {// while, with
                auto end = m_source.ConsumeWhile(IsIdentifier);
                auto image = m_source.GetString(start, end);

                return { GetKeyword(image), SourceRange(start, end) };
            }

            // Newlines
            case u'\r': case u'\n':
                type = TokenType::EndOfLine;
                break;

            // Punctuators
            case u'+':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceAdd : TokenType::Add;
                break;

            case u'-':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceSub : TokenType::Sub;
                break;

            case u'*':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceMul : TokenType::Mul;
                break;

            case u'/':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceDiv : TokenType::Div;
                break;

            case u'%':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceMod : TokenType::Mod;
                break;

            case u'&':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceBitAnd : TokenType::BitAnd;
                break;

            case u'|':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceBitOr : TokenType::BitOr;
                break;

            case u'^':
                type = m_source.ConsumeIf(u'=') ? TokenType::InplaceBitXor : TokenType::BitXor;
                break;

            case u'~':
                type = TokenType::BitNot;
                break;

            case u'=':
                if (m_source.ConsumeIf(u'=')) {
                    type = TokenType::Equal;
                }
                else if (m_source.ConsumeIf(u'>')) {
                    type = TokenType::Arrow;
                }
                else {
                    type = TokenType::Assign;
                }
                break;

            case u'!':
                if (m_source.ConsumeIf(u'=')) {
                    type = TokenType::NotEqual;
                }
                else {
                    m_reporter.Report(start, ReportID::LexUnknownToken)
                              .Note(ReportID::LexIntendedToNotEqual);
                    continue;
                }
                break;

            case u'<':
                if (m_source.ConsumeIf(u'=')) {
                    type = TokenType::LessOrEqual;
                }
                else if (m_source.ConsumeIf(u'<')) {
                    type = m_source.ConsumeIf(u'=') ? TokenType::InplaceLeftShift : TokenType::LeftShift;
                }
                else {
                    type = TokenType::Less;
                }
                break;

            case u'>':
                if (m_source.ConsumeIf(u'=')) {
                    type = TokenType::GraterOrEqual;
                }
                else if (m_source.ConsumeIf(u'>')) {
                    type = m_source.ConsumeIf(u'=') ? TokenType::InplaceRightShift : TokenType::RightShift;
                }
                else {
                    type = TokenType::Grater;
                }
                break;

            case u',':
                type = TokenType::Comma;
                break;

            case u':':
                type = TokenType::Colon;
                break;

            case u'.':
                if (m_source.ConsumeIf(u'.')) {
                    if (m_source.ConsumeIf(u'.')) {
                        type = TokenType::Ellipsis;
                    }
                    else {
                        m_reporter.Report(start, ReportID::LexUnknownToken)
                                  .Note(ReportID::LexIntendedToEllipsis);
                        continue;
                    }
                }
                else {
                    type = TokenType::Dot;
                }
                break;

            case u'(':
                type = TokenType::LeftParen;
                break;

            case u')':
                type = TokenType::RightParen;
                break;

            case u'{':
                type = TokenType::LeftBrace;
                break;

            case u'}':
                type = TokenType::RightBrace;
                break;

            case u'[':
                type = TokenType::LeftSquare;
                break;

            case u']':
                type = TokenType::RightSquare;
                break;
        } // end of switch

        return { type, SourceRange(start, m_source.GetPosition()) };
    } // end of infinite loop
}

Token Lexer::LexInteger(SourcePosition start, bool (*matcher)(char32_t), ReportID id) {
    m_source.ConsumeChar(); // Consume prefix

    SourcePosition end = m_source.ConsumeWhile(matcher);
    TokenType type = TokenType::Integer;

    if ((end.Column - start.Column) == 2) { // Only prefixes
        m_reporter.Report(end, id);
        type = TokenType::Invalid;
    }

    return { type, SourceRange(start, end) };
}

Token Lexer::LexNumber(SourcePosition start) {
    // Binary, octal and hexadecimal integers are already handled.
    // Only decimal integers and floating point numbers are considered.
    auto end = m_source.ConsumeWhile(IsDecimal);
    auto type = TokenType::Integer;

    if (m_source.ConsumeIf(u'.')) {
        if (!IsDecimal(m_source.PeekChar())) {
            goto InvalidToken;
        }
        else {
            end = m_source.ConsumeWhile(IsDecimal);
            type = TokenType::Float;
        }
    }

    if (m_source.ConsumeIf(u'e') || m_source.ConsumeIf(u'E')) {
        if (m_source.PeekChar() == u'+' || m_source.PeekChar() == u'-') {
            m_source.ConsumeChar();
        }

        if (!IsDecimal(m_source.PeekChar())) {
            goto InvalidToken;
        }
        else {
            end = m_source.ConsumeWhile(IsDecimal);
            type = TokenType::Float;
        }
    }

    return { type, SourceRange(start, end) };

InvalidToken:
    end = m_source.GetPosition();
    m_reporter.Report(end, ReportID::LexIncompleteExponent);

    return { TokenType::Invalid, SourceRange(start, end) };
}