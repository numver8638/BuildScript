/*
 * Parser.Statement.cpp
 * - Syntax analyzer for BuildScript language.
 *   This file contains partial code of parser that parses statement.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Parser.h>

#include <cassert>

#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/ErrorReporter.ReportID.h>

using namespace BuildScript;

inline bool IsAssignOp(const Token& token) {
    return (TokenType::Assign <= token.Type) && (token.Type <= TokenType::InplaceRightShift);
}

inline AssignOp ToAssignOp(const Token& token) {
    assert(IsAssignOp(token));

    static AssignOp table[] = {
        AssignOp::Assign,
        AssignOp::Add,
        AssignOp::Sub,
        AssignOp::Mul,
        AssignOp::Div,
        AssignOp::Mod,
        AssignOp::LeftShift,
        AssignOp::RightShift,
        AssignOp::BitAnd,
        AssignOp::BitOr,
        AssignOp::BitXor
    };

    return table[static_cast<size_t>(token.Type) - static_cast<size_t>(TokenType::Assign)];
}

ASTNode* Parser::ParseLocalDeclarationOrStatement() {
    switch (m_token.Type) {
        case TokenType::Var:
        case TokenType::Const:
        case TokenType::Static:
            return ParseVariableDeclaration();

        case TokenType::Export:
            return ParseExportDeclaration();

        case TokenType::Import:
            return ParseImportDeclaration();

        default:
            return ParseStatement();
    }
}

ASTNode* Parser::ParseStatement() {
    switch (m_token.Type) {
        case TokenType::If:
            return ParseIfStatement();

        case TokenType::Match:
            return ParseMatchStatement();

        case TokenType::For:
            return ParseForStatement();

        case TokenType::While:
            return ParseWhileStatement();

        case TokenType::With:
            return ParseWithStatement();

        case TokenType::Try:
            return ParseTryStatement();

        case TokenType::Break:
            return ParseBreakStatement();

        case TokenType::Continue:
            return ParseContinueStatement();

        case TokenType::Return:
            return ParseReturnStatement();

        case TokenType::Assert:
            return ParseAssertStatement();

        case TokenType::Pass:
            return ParsePassStatement();

        case TokenType::LeftBrace:
            return ParseBody();

        default: {
            auto* expr = ParseExpression();

            if (IsAssignOp(m_token)) {
                /*
                 * assignment_statement
                 *  : expr assign_op expr EOL
                 *  ;
                 *
                 * assign_op
                 *  : '='
                 *  | '+=' | '-=' | '*=' | '/=' | '%='
                 *  | '&=' | '^=' | '|='
                 *  | '<<=' | '>>='
                 *  ;
                 */
                auto op = ToAssignOp(m_token);
                auto opPos = ConsumeToken();
                auto* value = ParseExpression();

                RequireEOL();

                return AssignStatement::Create(m_context, expr, op, opPos, value);
            }
            else {
                /*
                 * expression_statement
                 *  : expr EOL
                 */
                RequireEOL();

                return expr;
            }
        }
    }
}

/*
 * body
 *  : block_statement
 *  | '=>' expression EOL   // arrow body
 *  | '=>' pass EOL         // empty arrow body
 *  ;
 *
 * block_statement
 *  : '{' statement* '}'
 *  ;
 */
Statement* Parser::ParseBody() {
    if (m_token == TokenType::Arrow) {
        auto arrow = ConsumeToken();

        Expression* body;

        if (m_token == TokenType::Pass) {
            body = PassExpression::Create(m_context, ConsumeToken());
        }
        else {
            body = ParseExpression();
        }

        RequireEOL();

        return ArrowStatement::Create(m_context, arrow, body);
    }
    else {
        auto open = RequireToken(TokenType::LeftBrace);

        std::vector<ASTNode*> nodes;
        while (!OneOf<TokenType::RightBrace, TokenType::EndOfFile>()) {
            auto* node = ParseLocalDeclarationOrStatement();
            nodes.push_back(node);
        }

        auto close = RequireToken(TokenType::RightBrace);

        return BlockStatement::Create(m_context, open, nodes, close);
    }
}

/*
 * if_statement
 *  : 'if' boolean_expression body else_statement?
 *  ;
 *
 * else_statement
 *  : 'else' body
 *  | 'else' if_statement
 *  ;
 */
Statement* Parser::ParseIfStatement() {
    assert(m_token == TokenType::If);

    auto _if = ConsumeToken();
    SourcePosition _else;
    auto* cond = ParseBooleanExpression();
    auto* body = ParseBody();
    Statement* elseBody = nullptr;

    // Parse else_statement if exists.
    if (ConsumeIf(TokenType::Else, _else)) {
        elseBody = (m_token == TokenType::If) ? ParseIfStatement() : ParseBody();
    }

    return IfStatement::Create(m_context, _if, cond, body, _else, elseBody);
}

/*
 * labeled_statement
 *  : label+ statement+
 *  ;
 *
 * label
 *  : 'case' constant ':'
 *  | 'default' ':'
 *  ;
 */
Statement* Parser::ParseLabeledStatement() {
    std::vector<Label*> labels;
    std::vector<ASTNode*> nodes;

    // Parse labels.
    while (OneOf<TokenType::Case, TokenType::Default>()) {
        SourcePosition _case, _default;
        auto isCase = (m_token == TokenType::Case);
        isCase ? (_case = ConsumeToken()) : (_default = ConsumeToken());

        Expression* constant = isCase ? ParseConstantExpression() : nullptr;
        auto colon = RequireToken(TokenType::Colon);

        labels.push_back(Label::Create(m_context, _case, _default, constant, colon));
    }

    // Validity check - labels must be present before statements.
    if (labels.empty()) {
        m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectLabel);
    }

    // Parse statements after labels.
    while (!OneOf<TokenType::Case, TokenType::Default, TokenType::RightBrace, TokenType::EndOfFile>()) {
        nodes.push_back(ParseLocalDeclarationOrStatement());
    }

    // Validity check - least 1 statement must be present after label.
    if (nodes.empty()) {
        m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectStatement);
    }

    return LabeledStatement::Create(m_context, labels, nodes);
}

/*
 * match_statement
 *  : 'match' expression '{' labeled_statement+ '}'
 *  ;
 */
Statement* Parser::ParseMatchStatement() {
    assert(m_token == TokenType::Match);

    auto match = ConsumeToken();
    auto* expr = ParseExpression();
    auto open = RequireToken(TokenType::LeftBrace);

    std::vector<Statement*> nodes;
    while (!OneOf<TokenType::RightBrace, TokenType::EndOfFile>()) {
        nodes.push_back(ParseLabeledStatement());
    }

    auto close = RequireToken(TokenType::RightBrace);

    return MatchStatement::Create(m_context, match, expr, open, nodes, close);
}

/*
 * for_statement
 *  : 'for' identifier 'in' expression body
 *  ;
 */
Statement* Parser::ParseForStatement() {
    assert(m_token == TokenType::For);

    auto _for = ConsumeToken();
    auto param = ParseParameter();
    auto in = RequireToken(TokenType::In);
    auto* expr = ParseExpression();
    auto* body = ParseBody();

    return ForStatement::Create(m_context, _for, param, in, expr, body);
}

/*
 * while_statement
 *  : 'while' boolean_expression body
 *  ;
 */
Statement* Parser::ParseWhileStatement() {
    assert(m_token == TokenType::While);

    auto _while = ConsumeToken();
    auto* cond = ParseBooleanExpression();
    auto* body = ParseBody();

    return WhileStatement::Create(m_context, _while, cond, body);
}

/*
 * with_statement
 *  : 'with' expression ('as' identifier)? body
 *  ;
 */
Statement* Parser::ParseWithStatement() {
    assert(m_token == TokenType::With);

    auto with = ConsumeToken();
    auto* expr = ParseExpression();
    SourcePosition as;
    Parameter* capture = nullptr;
    Statement* body;

    if (ConsumeIf(TokenType::As, as)) {
        capture = ParseParameter();
    }

    body = ParseBody();

    return WithStatement::Create(m_context, with, expr, as, capture, body);
}

/*
 * try_statement
 *  : 'try' body try_handler
 *  ;
 *
 * try_handler
 *  : except_clause+ finally_clause?
 *  | except_clause* finally_clause
 *  ;
 *
 * except_clause
 *  : 'except' typename ('as' identifier)? body
 *  ;
 *
 * finally_clause
 *  : 'finally' body
 *  ;
 */
Statement* Parser::ParseTryStatement() {
    assert(m_token == TokenType::Try);

    const auto ParseExceptClause = [&]() -> ExceptStatement* {
        SourcePosition except;

        if (ConsumeIf(TokenType::Except, except)) {
            auto _typename = RequireIdentifier();
            SourcePosition as;
            Parameter* capture = nullptr;
            Statement* body;

            if (ConsumeIf(TokenType::As, as)) {
                capture = ParseParameter();
            }

            body = ParseBody();

            return ExceptStatement::Create(m_context, except, std::move(_typename), as, capture, body);
        }
        else {
            return nullptr;
        }
    };
    const auto ParseFinallyClause = [&]() -> FinallyStatement* {
        SourcePosition finally;

        if (ConsumeIf(TokenType::Finally, finally)) {
            auto* body = ParseBody();

            return FinallyStatement::Create(m_context, finally, body);
        }
        else {
            return nullptr;
        }
    };

    auto try_ = ConsumeToken();
    std::vector<Statement*> handlers;

    handlers.push_back(ParseBody());

    while (auto* except = ParseExceptClause()) {
        handlers.push_back(except);
    }

    if (auto* finally = ParseFinallyClause()) {
        handlers.push_back(finally);
    }

    if (handlers.size() == 1) {
        m_reporter.Report(try_, ReportID::ParseNoTryHandler);
    }

    return TryStatement::Create(m_context, try_, handlers);
}

/*
 * break_statement
 *  : 'break' ('if' boolean_expression)? EOL
 *  ;
 */
Statement* Parser::ParseBreakStatement() {
    assert(m_token == TokenType::Break);

    // Consume token with range because 'break' may be used solely.
    auto _break = ConsumeToken();
    SourcePosition _if;
    Expression* cond = nullptr;

    // Check there's an EOL between 'break' and 'if'.
    // Only accept 'if' as condition when there's no EOL between 'break' and 'if'.
    if (!HasEOL() && ConsumeIf(TokenType::If, _if)) {
        cond = ParseBooleanExpression();
    }

    RequireEOL();

    return BreakStatement::Create(m_context, _break, _if, cond);
}

/*
 * continue_statement
 *  : 'continue' ('if' boolean_expression)? EOL
 *  ;
 */
Statement* Parser::ParseContinueStatement() {
    assert(m_token == TokenType::Continue);

    // Consume token with range because 'continue' may be used solely.
    auto _continue = ConsumeToken();
    SourcePosition _if;
    Expression* cond = nullptr;

    // Check there's an EOL between 'continue' and 'if'.
    // Only accept 'if' as condition when there's no EOL between 'continue' and 'if'.
    if (!HasEOL() && ConsumeIf(TokenType::If, _if)) {
        cond = ParseBooleanExpression();
    }

    RequireEOL();

    return ContinueStatement::Create(m_context, _continue, _if, cond);
}

/*
 * return_statement
 *  : 'return' expression? EOL
 *  ;
 */
Statement* Parser::ParseReturnStatement() {
    assert(m_token == TokenType::Return);

    // Consume token with range because 'return' may be used solely.
    auto _return = ConsumeToken();
    Expression* value = nullptr;

    if (!HasEOL()) {
        // Expression between 'return' and EOL is treated as return value.
        value = ParseExpression();
    }

    RequireEOL();

    return ReturnStatement::Create(m_context, _return, value);
}

/*
 * assert_statement
 *  : 'assert' boolean_expression (':' expression)? EOL
 *  ;
 */
Statement* Parser::ParseAssertStatement() {
    assert(m_token == TokenType::Assert);

    auto _assert = ConsumeToken();
    auto* cond = ParseBooleanExpression();
    SourcePosition colon;
    Expression* message = nullptr;

    if (ConsumeIf(TokenType::Colon, colon)) {
        message = ParseExpression();
    }

    RequireEOL();

    return AssertStatement::Create(m_context, _assert, cond, colon, message);
}

/*
 * pass_statement
 *  : 'pass' EOL
 *  ;
 */
Statement* Parser::ParsePassStatement() {
    assert(m_token == TokenType::Pass);

    return PassStatement::Create(m_context, ConsumeToken());
}