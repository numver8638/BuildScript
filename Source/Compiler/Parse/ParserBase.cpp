/*
 * ParserBase.cpp
 * - Base class of the parser.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/ParserBase.h>

#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/SourceText.h>

using namespace BuildScript;

ParserBase::ParserBase(SourceText& source, ErrorReporter& reporter)
    : m_source(source), m_reporter(reporter), m_lexer(source, reporter) {
    // Load first token.
    Consume();
}

void ParserBase::Consume() {
    // Clear newline flag
    m_newline = false;

    // Save previous token's range.
    m_prevRange = m_token.Range;

    do {
        if (!m_buffer.empty()) {
            m_token = m_buffer.front();
            m_buffer.pop_front();
        }
        else {
            m_token = m_lexer.GetNextToken();
        }

        if (m_token == TokenType::EndOfLine || m_token == TokenType::EndOfFile) {
            m_newline = true;
        }
    } while (m_token == TokenType::Comment || m_token == TokenType::EndOfLine);
}

const Token& ParserBase::PeekToken(int lookahead) {
    if (lookahead == 0) {
        return m_token;
    }
    else {
        const auto ShouldReturn = [&lookahead](const auto& tok) -> bool {
            // EOL and comments are not counted as lookahead count.
            if (tok == TokenType::EndOfLine || tok == TokenType::Comment) {
                return false;
            }
            else if (tok == TokenType::EndOfFile) {
                // edge case: file reached the end.
                return true;
            }
            else {
                return --lookahead == 0;
            }
        };

        for (const auto& tok : m_buffer) {
            if (ShouldReturn(tok)) {
                return tok;
            }
        }

        // Code reaches here means need more tokens to peek desired token.
        do {
            m_buffer.push_back(m_lexer.GetNextToken());
            const auto& tok = m_buffer.back();

            if (ShouldReturn(tok)) {
                return tok;
            }
        } while (true);
    }
}

SourcePosition ParserBase::RequireToken(TokenType expected) {
    if (m_token == expected) {
        return ConsumeToken();
    }
    else {
        if (m_token == TokenType::EndOfFile) {
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseUnexpectedEOF);
        }
        // Suppress error when token is invalid.
        else if (m_token != TokenType::Invalid) {
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectToken, Token::TypeToString(expected))
                      .Insert(m_token.GetPosition(), Token::TypeToString(expected));
        }

        return {};
    }
}

inline bool IsContextualKeyword(TokenType type) {
    switch (type) {
        default: return false;

        case TokenType::Inputs:
        case TokenType::Outputs:
        case TokenType::From:
        case TokenType::Do:
        case TokenType::DoFirst:
        case TokenType::DoLast:
        case TokenType::DependsOn:
            return true;
    }
}

Identifier ParserBase::RequireIdentifier() {
    if (m_token == TokenType::Identifier || IsContextualKeyword(m_token.Type)) {
        auto image = m_source.GetString(m_token.Range);
        auto range = ConsumeTokenRange();

        return { range, std::move(image) };
    }
    // Suppress error when token is invalid.
    else if (m_token != TokenType::Invalid) {
        auto reportID =
            (m_token == TokenType::EndOfFile) ? ReportID::ParseUnexpectedEOF : ReportID::ParseExpectIdentifier;

        m_reporter.Report(m_token.GetPosition(), reportID);
    }

    return {};
}

void ParserBase::RequireEOL() {
    if (!m_newline) {
        m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectNewLine)
                  .Insert(m_token.GetPosition(), "<enter>");
    }
}