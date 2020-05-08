/*
 * Lexer.cpp
 * - Tokenizer for BuildScript language.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Lexer.h>

#include <cassert>

#include <BuildScript/Compiler/CharType.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/SourceText.h>

constexpr int MAX_INTERPOLATION_DEPTH = 128;
constexpr int MODE = 0;
constexpr int QUOTE = 1;

using namespace BuildScript;

struct {
    const char* const Image;
    const TokenType Type;
} KeywordTable[] = {
    { u8"and",       TokenType::And },
    { u8"as",        TokenType::As },
    { u8"assert",    TokenType::Assert },
    { u8"case",      TokenType::Case },
    { u8"class",     TokenType::Class },
    { u8"continue",  TokenType::Continue },
    { u8"def",       TokenType::Def },
    { u8"default",   TokenType::Default },
    { u8"defined",   TokenType::Defined },
    { u8"deinit",    TokenType::Deinit },
    { u8"dependsOn", TokenType::DependsOn },
    { u8"do",        TokenType::Do },
    { u8"doFirst",   TokenType::DoFirst },
    { u8"doLast",    TokenType::DoLast },
    { u8"break",     TokenType::Break },
    { u8"else",      TokenType::Else },
    { u8"except",    TokenType::Except },
    { u8"export",    TokenType::Export },
    { u8"extends",   TokenType::Extends },
    { u8"false",     TokenType::False },
    { u8"finally",   TokenType::Finally },
    { u8"for",       TokenType::For },
    { u8"from",      TokenType::From },
    { u8"get",       TokenType::Get },
    { u8"if",        TokenType::If },
    { u8"import",    TokenType::Import },
    { u8"in",        TokenType::In },
    { u8"init",      TokenType::Init },
    { u8"inputs",    TokenType::Inputs },
    { u8"is",        TokenType::Is },
    { u8"match",     TokenType::Match },
    { u8"none",      TokenType::None },
    { u8"not",       TokenType::Not },
    { u8"operator",  TokenType::Operator },
    { u8"or",        TokenType::Or },
    { u8"outputs",   TokenType::Outputs },
    { u8"pass",      TokenType::Pass },
    { u8"raise",     TokenType::Raise },
    { u8"return",    TokenType::Return },
    { u8"self",      TokenType::Self },
    { u8"set",       TokenType::Set },
    { u8"static",    TokenType::Static },
    { u8"super",     TokenType::Super },
    { u8"task",      TokenType::Task },
    { u8"true",      TokenType::True },
    { u8"try",       TokenType::Try },
    { u8"value",     TokenType::Value },
    { u8"var",       TokenType::Var },
    { u8"while",     TokenType::While },
    { u8"with",      TokenType::With },
};

Token Lexer::Invalid(ReportID id) {
    auto pos = m_source.GetPosition();

    m_reporter.Report(pos, id);
    m_source.Consume();

    return Token(TokenType::Invalid, pos);
}

Lexer::Lexer(SourceText& source, ErrorReporter& reporter)
    : m_source(source), m_reporter(reporter) {}

/*
 * Lexer has 4 internal states:
 * - Default:
 *   - Default mode is a default state of lexer.
 *   - When the lexer meets '\'' or '"', switches into String mode.
 * - String:
 *   - In this mode, the lexer accepts all characters except '\'', '"', EndOfLine('\r', '\n', '\r\n')
 *     and EndOfFile as string.
 *   - If the lexer meets '$' which is not after '\\', switches into InterpolateBegin mode
 *     and returns partial string ranged beginning to before '$'.
 * - InterpolateBegin:
 *   - In this mode, the lexer distinguishes InterpolatedVar - like '$id' - and InterpolateBegin - '${'.
 *   - If the lexer finds '{' just after '$', switches into Interpolate mode and returns
 *     TokenType::InterpolateBegin token.
 *   - Otherwise, the lexer consumes identifier, switches into String mode and returns identifier
 *     as TokenType::InterpolatedVar.
 * - Interpolate:
 *   - Interpolate mode is similar with default mode but in string.
 *   - When the lexer finds matching '}', switches String mode and returns TokenType::InterpolateEnd token.
 *   - In this mode, EndOfLine and Comment are not allowed.
 *   - Maximum depth of interpolated expression is 128.
 *   - When maximum depth is reached, inner interpolated expressions are ignored and returned as TokenType::Invalid.
 */
Token Lexer::LexToken() {
    switch (m_mode) {
        case LexerMode::Default:
            return LexTokenInternal();
        
        case LexerMode::Interpolate: {
            Token token = LexTokenInternal();

            // Trace braces to distinguish which closing brace is
            // end marker of interpolated expression.
            if (token == TokenType::LeftBrace) {
                m_depth.push_back(/*InterpolateBegin=*/false);
            }

            if (token == TokenType::RightBrace) {
                if (/*InterpolateBegin=*/m_depth.back()) {
                    token.Type = TokenType::InterpolateEnd;

                    // Switch to String mode.
                    m_mode = LexerMode::String;
                }

                m_depth.pop_back();
            }

            return token;
        }

        case LexerMode::InterpolateBegin:
            return LexInterpolateBegin();

        case LexerMode::String: {
            int quote = std::get<QUOTE>(m_states.back());
            return LexString(quote);
        }
    }
}

Token Lexer::LexTokenInternal() {
    // Skip whitespaces
    m_source.ConsumeWhile(IsWhitespace);

    auto ch = m_source.PeekChar();

    switch (ch) {
        case SourceText::EndOfFile:
            return Token(TokenType::EndOfFile, m_source.GetPosition());

        case SourceText::InvalidEncoding:
            return Invalid(ReportID::LexInvalidEncoding);

        case SourceText::InvalidCharacter:
            return Invalid(ReportID::LexInvalidCharacter);

        // Keywords & Identifiers
        default:
            // Check character is in acceptable unicode range.
            // Otherwise, regard as punctuator and try lex it.
            if (!IsAllowedUnicode(ch))
                return LexPunctuator();
            else if (IsNotAllowedUnicodeInitially(ch))
                return Invalid(ReportID::LexNotAllowedCharacter);
            /*[[fallthrough]]*/

        case u'_':
        case u'a': case u'b': case u'c': case u'd': case u'e': case u'f': case u'g':
        case u'h': case u'i': case u'j': case u'k': case u'l': case u'm': case u'n':
        case u'o': case u'p': case u'q': case u'r': case u's': case u't': case u'u':
        case u'v': case u'w': case u'x': case u'y': case u'z':
        case u'A': case u'B': case u'C': case u'D': case u'E': case u'F': case u'G':
        case u'H': case u'I': case u'J': case u'K': case u'L': case u'M': case u'N':
        case u'O': case u'P': case u'Q': case u'R': case u'S': case u'T': case u'U':
        case u'V': case u'W': case u'X': case u'Y': case u'Z':
            return LexIdentifier();

        // Numbers
        case u'0': case u'1': case u'2': case u'3': case u'4':
        case u'5': case u'6': case u'7': case u'8': case u'9':
            return LexNumber();

        // Strings
        case u'"':
        case u'\'':
            return LexString(ch);

        // Comments
        case u'#':
            return LexComment();
    }

    assert(!"never reached");
}

Token Lexer::LexNumber() {
    assert(IsDecimal(m_source.PeekChar()));

    auto pos = m_source.GetPosition();

    auto fn = [&](bool (*expected)(int), ReportID id) -> bool {
        if (!expected(m_source.PeekChar())) {
            m_reporter.Report(/*pos=*/m_source.GetPosition(), id);
            m_source.Consume();
            return false;
        }
        else {
            m_source.ConsumeWhile(expected);
            return true;
        }
    };

    std::function<bool(void)> match;

    if (m_source.PeekChar() == u'0') {
        m_source.Consume();  // consume '0'

        switch (m_source.PeekChar()) {
            case u'b':
            case u'B':
                match = std::bind(fn, IsBinary, ReportID::LexExpectBinary);
                break;

            case u'o':
            case u'O':
                match = std::bind(fn, IsOctal, ReportID::LexExpectOctal);
                break;

            case u'x':
            case u'X':
                match = std::bind(fn, IsHexadecimal, ReportID::LexExpectHex);
                break;

            default:
                goto LexDecimal;
        }

        m_source.Consume(); // consume prefix

        if (match())
            return Token(TokenType::Integer, pos, StringRef(m_source, pos, /*end=*/m_source.GetPosition()));
        else
            goto HandleError;
    }

LexDecimal:
    m_source.ConsumeWhile(IsDecimal);

    if (m_source.PeekChar() == u'.' ||
        m_source.PeekChar() == u'e' || m_source.PeekChar() == u'E')
        goto LexFloat;
    else
        return Token(TokenType::Integer, pos, StringRef(m_source, pos, /*end=*/m_source.GetPosition()));

LexFloat:
    assert(m_source.PeekChar() == u'.' || m_source.PeekChar() == u'e' || m_source.PeekChar() == u'E');

    match = std::bind(fn, IsDecimal, ReportID::LexExpectExponent);

    if (m_source.ConsumeIf(u'.') && !match())
        goto HandleError;

    if (m_source.ConsumeIf(u'e') || m_source.ConsumeIf(u'E')) {
        if (m_source.PeekChar() == u'+' || m_source.PeekChar() == u'-')
            m_source.Consume();

        if (!match())
            goto HandleError;
    }


    return Token(TokenType::Float, pos, StringRef(m_source, pos, /*end=*/m_source.GetPosition()));

HandleError:
    return Token(TokenType::Invalid, pos);
}

// TODO: Use other algorithm rather than for loop to find keyword.
Token Lexer::LexIdentifier() {
    assert(IsIdentifier(m_source.PeekChar()));

    auto begin = m_source.GetPosition();
    auto end = m_source.ConsumeWhile(IsIdentifier);
    auto image = m_source.GetString(begin, end);
    auto type = TokenType::Identifier;

    for (auto& e : KeywordTable) {
        if (image == e.Image) {
            type = e.Type;
            break;
        }
    }

    return Token(type, begin, StringRef(m_source, begin, end));
}

Token Lexer::LexString(int quote) {
    assert(m_mode != LexerMode::InterpolateBegin);
    assert(quote == u'\'' || quote == u'"');

    auto pos = m_source.GetPosition();

    if (m_mode != LexerMode::String) {
        m_source.Consume(); // Consume quote

        // Save state of before String mode.
        m_states.emplace_back(m_mode, quote);

        m_mode = LexerMode::String;
    }

    auto begin = m_source.GetPosition();
    int ch;

    while ((ch = m_source.PeekChar()) != quote) {
        switch (ch) {
            case u'\r':
            case u'\n':
            case SourceText::EndOfFile:
                // Reset state
                m_mode = LexerMode::Default;
                m_states.clear();
                return Invalid(ReportID::LexUnexpectedEOL);

            case u'\\':
                HandleEscapeSequence();
                break;
            
            case u'$':
                // Switch to InterpolateBegin mode.
                m_mode = LexerMode::InterpolateBegin;

                return Token(TokenType::String,
                             pos,
                             StringRef(m_source, begin, /*end=*/m_source.GetPosition()));
            
            default:
                m_source.Consume();
                break;
        }
    }

    // Restore state
    m_mode = std::get<MODE>(m_states.back());
    m_states.pop_back();

    auto end = m_source.GetPosition();

    m_source.Consume(); // Consume quote

    return Token(TokenType::String, pos, StringRef(m_source, begin, end));
}

void Lexer::HandleEscapeSequence() {
    assert(m_source.PeekChar() == u'\\');

    m_source.Consume();
    int expected;

    switch (m_source.PeekChar()) {
        default:
            m_reporter.Report(m_source.GetPosition(), ReportID::LexInvalidEscape, m_source.PeekChar());
            /*[[fallthrough]]*/

        case u'a': case u'b': case u'f': case u'n': case u'r':
        case u't': case u'v': case u'\'': case u'"': case u'$': case u'\\':
            m_source.Consume();
            break;

        // Unicode escape sequence (UTF-16)
        case u'u':
            expected = 4;
            goto HandleEscape;

        // Unicode escape sequence (UTF-32)
        case u'U':
            expected = 8;
            goto HandleEscape;

        // Byte escape sequence - hexadecimal
        case u'x':
            expected = 2;
            goto HandleEscape;
        
        HandleEscape: {
            auto count = 0;
            do {
                m_source.Consume();
            } while (count++ < expected && IsHexadecimal(m_source.PeekChar()));

            if (count < expected)
                m_reporter.Report(/*pos=*/m_source.GetPosition(),
                                  ReportID::LexMoreDigitInEscapeSequence, expected, count);

            break;
        }

        // Byte escape sequence - octal
        case u'0': case u'1': case u'2': case u'3':
        case u'4': case u'5': case u'6': case u'7': {
            int loops = 3;

            do {
                m_source.Consume();
            } while (--loops > 0 && IsOctal(m_source.PeekChar()));

            break;
        }
    }
}

Token Lexer::LexPunctuator() {
    auto pos = m_source.GetPosition();
    auto ch = m_source.PeekChar();
    auto type = TokenType::Invalid;

    m_source.Consume();

    switch (ch) {
        case SourceText::EndOfFile:
            type = TokenType::EndOfFile;
            break;
        
        case SourceText::InvalidEncoding:
            m_reporter.Report(pos, ReportID::LexInvalidEncoding);
            break;

        case SourceText::InvalidCharacter:
            m_reporter.Report(pos, ReportID::LexInvalidCharacter);
            break;

        default:
            m_reporter.Report(pos, ReportID::LexUnknownToken);
            break;

        // Newlines
        case u'\r':
        case u'\n':
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
                m_reporter.Report(pos, ReportID::LexUnknownToken);
                m_reporter.Report(pos, ReportID::LexIntendToNotEqual);
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
                    m_reporter.Report(pos, ReportID::LexUnknownToken);
                    m_reporter.Report(pos, ReportID::LexIntendToEllipsis);
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
    }

    return Token(type, pos);
}

Token Lexer::LexComment() {
    auto pos = m_source.GetPosition();

    m_source.Consume(); // consume #

    auto begin = m_source.GetPosition();
    auto end = m_source.ConsumeWhile([](int ch) { return !IsEOL(ch); });

    if (m_mode != LexerMode::Default) {
        m_reporter.Report(pos, ReportID::LexCommentInInterpolatedString);
        return Token(TokenType::Invalid, pos);
    }
    else
        return Token(TokenType::Comment, pos, StringRef(m_source, begin, end));
}

static bool IsContextualKeyword(TokenType type) {
    return (type == TokenType::DependsOn) ||
           (type == TokenType::Get) ||
           (type == TokenType::Inputs) ||
           (type == TokenType::Outputs) ||
           (type == TokenType::Set) ||
           (type == TokenType::Task) ||
           (type == TokenType::Value);
}

Token Lexer::LexInterpolateBegin() {
    assert(m_mode == LexerMode::InterpolateBegin);
    assert(m_source.PeekChar() == u'$');

    auto pos = m_source.GetPosition();

    m_source.Consume(); // Consume $

    if (m_source.ConsumeIf(u'{')) {
        // Interpolated expression
        // Check max depth of interpolation is reached.
        if (m_states.size() > MAX_INTERPOLATION_DEPTH) {
            m_reporter.Report(pos, ReportID::LexMaxInterpolationDepthReached);

            // Error recovery
            int depth = 1;
            auto endOfInterpolation = [&depth](int ch) -> bool {
                switch (ch) {
                    case u'\r':
                    case u'\n':
                    case SourceText::EndOfFile:
                        // Reached at end of line. Stop consuming.
                        return false;
                    
                    case u'}':
                        return depth-- > 0;
                    
                    case u'{':
                        depth++;
                        /*[[fallthrough]]*/

                    default:
                        return depth > 0;
                }
            };

            auto end = m_source.ConsumeWhile(endOfInterpolation);

            // Revert to String mode.
            m_mode = LexerMode::String;
            return Token(TokenType::Invalid, pos);
        }

        // Switch to Interpolate mode.
        m_mode = LexerMode::Interpolate;
        m_depth.push_back(/*InterpolateBegin=*/true);

        return Token(TokenType::InterpolateBegin, pos);
    }
    else {
        // Interpolated variable
        if (!IsIdentifier(m_source.PeekChar())) {
            m_reporter.Report(m_source.GetPosition(), ReportID::LexInterpolatedVarExpected);
            
            // Revert to String mode.
            m_mode = LexerMode::String;
            return Token(TokenType::Invalid, pos);
        }
        
        auto token = LexIdentifier();

        if ((token != TokenType::Identifier) && !IsContextualKeyword(token.Type)) {
            m_reporter.Report(token.Position, ReportID::LexKeywordCannotBeInterpolatedVar, token.Image.ToString());

            token.Type = TokenType::Invalid;
        }
        else {
            token.Position = pos;
            token.Type = TokenType::InterpolatedVar;
        }

        // Revert to String mode.
        m_mode = LexerMode::String;
        return token;
    }
}