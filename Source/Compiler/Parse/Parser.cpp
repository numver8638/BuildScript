/*
 * Parser.cpp
 * - Syntax analyzer for BuildScript language.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Parser.h>

#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/SourceText.h>

using namespace BuildScript;

bool Parser::CheckForClosure() {
    assert(m_token == TokenType::LeftParen);

    // Case 1. ( id
    if (PeekToken(1) == TokenType::Identifier) {
        switch (PeekToken(2).Type) {
            case TokenType::Identifier: // Potential typo: Omitted comma
            case TokenType::Arrow: // Potential typo: Omitted right paren
            case TokenType::Comma:
                return /*isClosure=*/true;

            case TokenType::RightParen:
                // Case 1.1  ( id )
                // To distinguish closure and parenthesis, check '=>' after ')'.
                return PeekToken(3) == TokenType::Arrow;

            default:
                return /*isClosure=*/false;
        }
    }
    // Case 2. ( )
    else  {
        // Empty parenthesis considered as empty parameter list.
        return PeekToken(1) == TokenType::RightParen;
    }
}

SourceRange Parser::SkipToEOL() {
    auto begin = m_token.GetPosition();

    while (!HasEOL()) { ConsumeToken(); }

    return SourceRange::Merge(begin, m_prevRange);
}

SourceRange Parser::SkipTokenExpr() {
    switch (m_token.Type) {
        default:
        // Never be shown in parser.
        case TokenType::EndOfLine:
        case TokenType::Comment:

        // Always be handled in ParsePrimaryExpression().
        case TokenType::Identifier:
        case TokenType::Integer:
        case TokenType::Float:
        case TokenType::String:
        case TokenType::Defined:
        case TokenType::DependsOn:
        case TokenType::Do:
        case TokenType::DoFirst:
        case TokenType::DoLast:
        case TokenType::False:
        case TokenType::From:
        case TokenType::Inputs:
        case TokenType::None:
        case TokenType::Not:
        case TokenType::Outputs:
        case TokenType::Self:
        case TokenType::Super:
        case TokenType::True:
        case TokenType::LeftParen:
        case TokenType::LeftBrace:
        case TokenType::LeftSquare:
            NOT_REACHABLE;

        case TokenType::Invalid:
            return ConsumeTokenRange();

        // End of file
        case TokenType::EndOfFile:

        // Binary operators
        case TokenType::And:
        case TokenType::In:
        case TokenType::Is:
        case TokenType::Or:
        case TokenType::Add:
        case TokenType::Sub:
        case TokenType::Mul:
        case TokenType::Div:
        case TokenType::Mod:
        case TokenType::LeftShift:
        case TokenType::RightShift:
        case TokenType::BitAnd:
        case TokenType::BitOr:
        case TokenType::BitNot:
        case TokenType::BitXor:
        case TokenType::Less:
        case TokenType::LessOrEqual:
        case TokenType::Grater:
        case TokenType::GraterOrEqual:
        case TokenType::Equal:
        case TokenType::NotEqual:

        // Assignments
        case TokenType::Assign:
        case TokenType::InplaceAdd:
        case TokenType::InplaceSub:
        case TokenType::InplaceMul:
        case TokenType::InplaceDiv:
        case TokenType::InplaceMod:
        case TokenType::InplaceBitAnd:
        case TokenType::InplaceBitOr:
        case TokenType::InplaceBitXor:
        case TokenType::InplaceLeftShift:
        case TokenType::InplaceRightShift:

        // Statements
        case TokenType::Assert:
        case TokenType::Break:
        case TokenType::Const:
        case TokenType::Continue:
        case TokenType::For:
        case TokenType::If:
        case TokenType::Match:
        case TokenType::Pass:
        case TokenType::Raise:
        case TokenType::Return:
        case TokenType::Static:
        case TokenType::Try:
        case TokenType::Var:
        case TokenType::While:
        case TokenType::With:
            // Do not consume any token. These tokens have potentials to parse correctly.
            return SourceRange(m_token.GetPosition());

        // Labels
        case TokenType::Case:
        case TokenType::Default: {
            auto begin = m_token.GetPosition();
            SkipUntil<TokenType::Colon, TokenType::RightBrace>();

            return SourceRange::Merge(begin, m_prevRange);
        }

        // Statements that are trailing or declarations
        case TokenType::Else:
        case TokenType::Except:
        case TokenType::Finally:
        case TokenType::Extends:
        case TokenType::Class:
        case TokenType::Def:
        case TokenType::Deinit:
        case TokenType::Get:
        case TokenType::Init:
        case TokenType::Set:
        case TokenType::Task:
            return SkipBraces(StopAfterBrace);

        case TokenType::Export:
        case TokenType::Import:
            return SkipToEOL();

        case TokenType::As:
        case TokenType::Comma:
        case TokenType::Colon:
        case TokenType::Dot:
        case TokenType::Ellipsis:
        case TokenType::Arrow:
        case TokenType::RightParen:
        case TokenType::RightBrace:
        case TokenType::RightSquare:
            return ConsumeTokenRange();
    }
}

SourceRange Parser::SkipBraces(SkipFlag flag, const Predicate<TokenType>& filter) {
    auto begin = m_token.GetPosition();
    auto depth = 0;

    while (true) {
        if (m_token == TokenType::EndOfFile || (depth == 0 && filter(m_token.Type))) {
            break;
        }
        else if (m_token == TokenType::LeftBrace) {
            ++depth;
        }
        else if (m_token == TokenType::RightBrace) {
            --depth;

            if (flag == StopBeforeBrace && depth < 0) {
                break;
            }
            else if (flag == StopAfterBrace && depth <= 0) {
                ConsumeToken();
                break;
            }
        }

        ConsumeToken();
    }

    return SourceRange::Merge(begin, m_prevRange);
}

/*
 * script
 *  : declaration_or_statement* EOF
 *  ;
 *
 * declaration_or_statement
 *  : declaration
 *  | statement
 *  ;
 */
ASTNode* Parser::Parse() {
    std::vector<ASTNode*> nodes;

    while (m_token != TokenType::EndOfFile) {
        if (auto* decl = ParseDeclaration()) {
            nodes.push_back(decl);
        }
        else {
            auto* stmt = ParseStatement();
            assert(stmt != nullptr);

            nodes.push_back(stmt);
        }
    }

    return ScriptDeclaration::Create(m_context, m_source.GetName(), nodes);
}