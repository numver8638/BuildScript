/*
 * Parser.Misc.cpp
 * - Syntax analyzer for BuildScript language.
 *   This file contains partial code of parser that parses expression.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Parser.h>

#include <BuildScript/Compiler/AST/Parameters.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/ErrorReporter.ReportID.h>

using namespace BuildScript;

Parameters* Parser::ParseParameters() {
    auto open = RequireToken(TokenType::LeftParen);
    std::vector<Identifier> names;
    std::vector<SourcePosition> commas;

    SourcePosition ellipsis;

    if (m_token != TokenType::RightParen) {
        ParseNameList(names, commas);
    }

    ConsumeIf(TokenType::Ellipsis, ellipsis);

    auto close = RequireToken(TokenType::RightParen);

    return Parameters::Create(m_context, open, names, commas, ellipsis, close);
}

void Parser::ParseNameList(std::vector<Identifier>& names, std::vector<SourcePosition>& commas) {
    bool hasComma;

    do {
        auto name = RequireIdentifier();
        SourcePosition comma;

        hasComma = ConsumeIf(TokenType::Comma, comma);

        if (!hasComma && m_token == TokenType::Identifier) {
            // Common typo: omitted comma.
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseOmittedComma)
                      .Insert(m_token.GetPosition(), Token::TypeToString(TokenType::Comma));

            // Continue parsing as if there's a comma.
            hasComma = true;
        }

        names.push_back(name);
        if (hasComma) {
            commas.push_back(comma);
        }
    } while (hasComma);
}

static inline bool IsStartOfExpression(const Token& token) {
    switch (token.Type) {
        // Primary expressions
        case TokenType::Inputs:
        case TokenType::Outputs:
        case TokenType::From:
        case TokenType::Do:
        case TokenType::DoFirst:
        case TokenType::DoLast:
        case TokenType::DependsOn:
        case TokenType::Identifier:
        case TokenType::Integer:
        case TokenType::Float:
        case TokenType::String:
        case TokenType::LeftBrace:
        case TokenType::LeftParen:
        case TokenType::LeftSquare:
        case TokenType::True:
        case TokenType::False:
        case TokenType::None:
        case TokenType::Self:
        case TokenType::Super:

        // Unary expressions
        case TokenType::Defined:
        case TokenType::Raise:
        case TokenType::Add:
        case TokenType::Sub:
        case TokenType::Not:
        case TokenType::BitNot:
            return true;

        default:
            return false;
    }
}

void Parser::ParseExpressionList(std::vector<Expression*>& exprs, std::vector<SourcePosition>& commas) {
    bool hasComma;

    do {
        auto* expr = ParseExpression();
        SourcePosition comma;

        hasComma = ConsumeIf(TokenType::Comma, comma);
        if (!hasComma && IsStartOfExpression(m_token)) {
            // Common typo: omitted comma.
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseOmittedComma)
                      .Insert(m_token.GetPosition(), Token::TypeToString(TokenType::Comma));

            // Continue parsing as if there's a comma.
            hasComma = true;
        }

        exprs.push_back(expr);
        if (hasComma) {
            commas.push_back(comma);
        }
    } while (hasComma);
}