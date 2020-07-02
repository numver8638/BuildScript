/*
 * Parser.cpp
 * - Syntax analyzer (a.k.a parser) for BuildScript language.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parser.h>

#include <BuildScript/Compiler/AST/ScriptNode.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/SourceText.h>

using namespace BuildScript;

Parser::Parser(SourceText& source, ErrorReporter& reporter)
    : m_filename(source.GetFileName()), m_lexer(source, reporter), m_reporter(reporter) {}

ScriptNode* Parser::ParseScript() {
    auto script = new ScriptNode(m_filename);

    GetNextToken(); // Load first token

    while (m_token != TokenType::EndOfFile) {
        ASTNode* node;
        auto decl = ParseDeclaration();

        if (decl.HasValue()) {
            node = decl.GetValue();
        }
        else {
            node = ParseStatement().GetValue();
        }

        script->Append(node);
    }

    return script;
}

SourcePosition Parser::ConsumeToken() {
    auto pos = m_token.Position;
    GetNextToken();
    return pos;
}

bool Parser::ConsumeIf(TokenType desired, SourcePosition* pos) {
    if (m_token == desired) {
        if (pos != nullptr)
            *pos = m_token.Position;

        GetNextToken();
        return true;
    }
    else
        return false;
}

void Parser::Expect(bool &error, TokenType expected, const std::function<void(const SourcePosition&)>& onfail) {
    auto accepted = (m_token == expected);

    if (accepted) {
        GetNextToken();
    }
    else {
        error = true;
        onfail(m_token.Position);
    }
}

void Parser::ExpectEOL(bool &error, const std::function<void()>& onfail) {
    auto hasEOL = m_newline;
    m_newline = false;

    if (!hasEOL) {
        error = true;
        m_reporter.Report(m_token.Position, ReportID::ParseExpectNewLine);

        if (onfail != nullptr)
            onfail();
    }
}

StringRef Parser::ExpectIdentifier(bool &error, SourcePosition* pos, const std::function<void()>& onfail) {
    StringRef image;

    if (m_token == TokenType::Identifier) {
        image = m_token.Image;

        if (pos != nullptr)
            *pos = m_token.Position;

        GetNextToken();
    }
    else {
        m_reporter.Report(m_token.Position, ReportID::ParseExpectIdentifier);

        if (onfail != nullptr)
            onfail();

        error = true;
    }

    return image;
}

void Parser::ConsumeInvalidStatements() {
    switch (m_token.Type) {
        case TokenType::Export:
        case TokenType::Import: {
            auto pos = m_token.Position;
            m_reporter.Report(pos, ReportID::ParseDeclarationInStatement);
            SkipToEOL();
            break;
        }

        case TokenType::Task:
        case TokenType::Class:
        case TokenType::Def: {
            auto pos = m_token.Position;
            m_reporter.Report(pos, ReportID::ParseDeclarationInStatement);
            SkipUntil(TokenType::RightBrace);
            ConsumeToken(); // TokenType::RightBrace
            break;
        }

        case TokenType::If:
        case TokenType::Else:
        case TokenType::Match:
        case TokenType::For:
        case TokenType::While:
        case TokenType::Try:
        case TokenType::Except:
        case TokenType::Finally: {
            auto pos = m_token.Position;
            m_reporter.Report(pos, ReportID::ParseInvalidPositionOfStatement);
            SkipUntil(TokenType::RightBrace);
            ConsumeToken(); // TokenType::RightBrace
            break;
        }

        case TokenType::Break:
        case TokenType::Continue:
        case TokenType::Return:
        case TokenType::Raise:
        case TokenType::Assert:
        case TokenType::Pass:
        case TokenType::Var:
        case TokenType::Case:
        case TokenType::Default:
        default: {
            auto pos = m_token.Position;
            m_reporter.Report(pos, ReportID::ParseInvalidPositionOfStatement);
            SkipToEOL();
            break;
        }
    }
}

