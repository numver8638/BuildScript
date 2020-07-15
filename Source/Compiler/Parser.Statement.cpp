/*
 * Parser.Statement.cpp
 * - Syntax analyzer (a.k.a parser) for BuildScript language.
 *   This file contains part of Parser that parse statements.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parser.h>
#include <BuildScript/Compiler/Parser.Shared.h>

#include <BuildScript/Compiler/AST/Statements.h>

using namespace BuildScript;

template <typename T, typename... Args>
StmtResult MakeResult(bool error, Args&&... args) {
    T* stmt = new T(std::forward<Args>(args)...);
    return StmtResult(error, stmt);
}

static inline bool IsAssignOp(TokenType type) {
    switch (type) {
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
            return true;

        default:
            return false;
    }
}

static inline AssignOp GetAssignOp(TokenType type) {
    switch (type) {
        case TokenType::Assign:
            return AssignOp::Assign;
        case TokenType::InplaceAdd:
            return AssignOp::InplaceAdd;
        case TokenType::InplaceSub:
            return AssignOp::InplaceSub;
        case TokenType::InplaceMul:
            return AssignOp::InplaceMul;
        case TokenType::InplaceDiv:
            return AssignOp::InplaceDiv;
        case TokenType::InplaceMod:
            return AssignOp::InplaceMod;
        case TokenType::InplaceBitAnd:
            return AssignOp::InplaceBitAnd;
        case TokenType::InplaceBitOr:
            return AssignOp::InplaceBitOr;
        case TokenType::InplaceBitXor:
            return AssignOp::InplaceBitXor;
        case TokenType::InplaceLeftShift:
            return AssignOp::InplaceLeftShift;
        case TokenType::InplaceRightShift:
            return AssignOp::InplaceRightShift;

        default:
            assert(!"never reached");
    }
}

StmtResult Parser::ParseStatement() {
    switch (m_token.Type) {
        case TokenType::If:
            return ParseIfStatement();

        case TokenType::Match:
            return ParseMatchStatement();

        case TokenType::For:
            return ParseForStatement();

        case TokenType::While:
            return ParseWhileStatement();

        case TokenType::Try:
            return ParseTryStatement();

        case TokenType::Raise:
            return ParseRaiseStatement();

        case TokenType::Break:
            return ParseBreakStatement();

        case TokenType::Continue:
            return ParseContinueStatement();

        case TokenType::Return:
            return ParseReturnStatement();

        case TokenType::Pass:
            return ParseEmptyStatement();

        case TokenType::Assert:
            return ParseAssertStatement();

        case TokenType::Var:
            return ParseVariableDeclaration();

        default: {
            // ExpressionStatement or AssignmentStatement
            StmtResult stmt;
            bool error = false;
            Expression* expr;
            Result(error, expr) = ParseExpression();

            if (IsAssignOp(m_token.Type)) {
                auto op = GetAssignOp(m_token.Type);
                Expression* lhs;

                ConsumeToken();
                Result(error, lhs) = ParseExpression();

                stmt = MakeResult<AssignmentStatement>(error, expr, lhs, op);
            }
            else
                stmt = StmtResult(error, expr);

            ExpectEOL(error, SkipToEOL);

            return stmt;
        }

        // Invalid tokens
        case TokenType::Export:
        case TokenType::Import:
        case TokenType::Task:
        case TokenType::Class:
        case TokenType::Def:
        case TokenType::Else:
        case TokenType::Except:
        case TokenType::Finally:
        case TokenType::Case:
        case TokenType::Default: {
            auto pos = m_token.Position;
            ConsumeInvalidStatements();
            return MakeResult<ErrorStatement>(true, pos);
        }
    }
}

StmtResult Parser::ParseBlock() {
    auto error = false;
    Statement* stmt;
    std::vector<Statement*> stmts;

    Expect(error, TokenType::LeftBrace, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseExpectBrace);
        
        SkipUntil(TokenType::LeftBrace, TokenType::RightBrace);
        ConsumeIf(TokenType::LeftBrace);
    });

    while (!ConsumeIf(TokenType::RightBrace)) {
        if (m_token == TokenType::EndOfFile) {
            m_reporter.Report(m_token.Position, ReportID::ParseBraceNotClosed);
            error = true;
            break;
        }

        Result(error, stmt) = ParseStatement();
        stmts.push_back(stmt);
    }

    return MakeResult<BlockStatement>(error, std::move(stmts));
}

StmtResult Parser::ParseIfStatement() {
    assert(m_token == TokenType::If);

    auto pos = ConsumeToken();
    auto error = false;
    std::map<Expression*, Statement*> thenClauses;
    Statement* elseClause = nullptr;

ParseElseIf:
    Expression* condition;
    Statement* body;
    
    Result(error, condition) = ParseBooleanExpression();
    Result(error, body) = ParseBlock();
    
    thenClauses.emplace(condition, body);

    if (ConsumeIf(TokenType::Else)) {
        if (ConsumeIf(TokenType::If))
            goto ParseElseIf;

        Result(error, elseClause) = ParseBlock();
    }

    return MakeResult<IfStatement>(error, pos, std::move(thenClauses), elseClause);
}

static inline bool IsEndOfLabeledStatement(TokenType type) {
    return type == TokenType::RightBrace || // End of match statement
           type == TokenType::EndOfFile ||  // End of file - error case
           type == TokenType::Case || type == TokenType::Default; // start of label
}

ParseResult<LabeledStatement> Parser::ParseLabeledStatement() {
    std::vector<LabeledStatement::Label> labels;
    std::vector<Statement*> stmts;
    auto error = false;

    if (m_token != TokenType::Case && m_token != TokenType::Default) {
        m_reporter.Report(m_token.Position, ReportID::ParseExpectLabel);
        error = true;
    }

    while (true) {
        auto pos = m_token.Position;
        Expression* value = nullptr;

        if (ConsumeIf(TokenType::Case)) {
            Result(error, value) = ParseBooleanExpression();
            Expect(error, TokenType::Colon, ExpectColon);
        }
        else if (ConsumeIf(TokenType::Default)) {
            Expect(error, TokenType::Colon, ExpectColon);
        }
        else 
            break;
        
        labels.emplace_back(pos, value);
    }

    if (m_token == TokenType::RightBrace) {
        m_reporter.Report(m_token.Position, ReportID::ParseRequireStatementInLabel);
        error = true;
    }
    else {
        while (!IsEndOfLabeledStatement(m_token.Type)) {
            Statement* stmt;
            Result(error, stmt) = ParseStatement();
            stmts.push_back(stmt);
        }
    }

    return ParseResult<LabeledStatement>(error, new LabeledStatement(std::move(labels), std::move(stmts)));
}

StmtResult Parser::ParseMatchStatement() {
    assert(m_token == TokenType::Match);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;

    Result(error, expr) = ParseExpression();
    std::vector<LabeledStatement*> stmts;

    if (error)
        SkipUntil(TokenType::LeftBrace);

    Expect(error, TokenType::LeftBrace, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseExpectBrace);
    });

    if (ConsumeIf(TokenType::RightBrace)) {
        m_reporter.Report(m_token.Position, ReportID::ParseEmptyMatchStatement);
        error = true;
    } else {
        do {
            if (m_token == TokenType::EndOfFile) {
                m_reporter.Report(m_token.Position, ReportID::ParseUnexpectedEOF);
                error = true;
                break;
            }

            LabeledStatement* stmt;
            Result(error, stmt) = ParseLabeledStatement();
            stmts.push_back(stmt);
        } while (!ConsumeIf(TokenType::RightBrace));
    }

    return MakeResult<MatchStatement>(error, pos, expr, std::move(stmts));
}

StmtResult Parser::ParseForStatement() {
    assert(m_token == TokenType::For);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;
    Statement* body;
    std::vector<ForStatement::Parameter> elements;

    do {
        SourcePosition paramPos;
        StringRef param = ExpectIdentifier(error, &paramPos);

        if (param)
            elements.emplace_back(param, paramPos);
    } while (ConsumeIf(TokenType::Comma));

    Expect(error, TokenType::In, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseExpectIn);
    });

    Result(error, expr) = ParseExpression();
    Result(error, body) = ParseBlock();

    return MakeResult<ForStatement>(error, pos, std::move(elements), expr, body);
}

StmtResult Parser::ParseWhileStatement() {
    assert(m_token == TokenType::While);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;
    Statement* body;

    Result(error, expr) = ParseExpression();
    Result(error, body) = ParseBlock();

    return MakeResult<WhileStatement>(error, pos, expr, body);
}

StmtResult Parser::ParseTryStatement() {
    assert(m_token == TokenType::Try);

    auto pos = ConsumeToken();
    SourcePosition exceptPos;
    auto error = false;
    Statement* body;
    std::vector<ExceptClause*> excepts;
    Statement* finally = nullptr;

    Result(error, body) = ParseBlock();

    while (ConsumeIf(TokenType::Except, &exceptPos)) {
        StringRef exceptType, exceptVar;
        Statement* exceptBody;

        exceptType = ExpectIdentifier(error, nullptr, [&]{ SkipUntil(TokenType::LeftBrace); });

        if (ConsumeIf(TokenType::As)) {
            exceptVar = ExpectIdentifier(error, nullptr, [&]{ SkipUntil(TokenType::LeftBrace); });
        }

        Result(error, exceptBody) = ParseBlock();

        excepts.push_back(new ExceptClause(exceptPos, exceptType, exceptVar, exceptBody));
    }

    if (ConsumeIf(TokenType::Finally))
        Result(error, finally) = ParseBlock();

    return MakeResult<TryStatement>(error, pos, body, std::move(excepts), finally);
}

StmtResult Parser::ParseRaiseStatement() {
    assert(m_token == TokenType::Raise);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;

    Result(error, expr) = ParseExpression();
    ExpectEOL(error);

    return MakeResult<RaiseStatement>(error, pos, expr);
}

StmtResult Parser::ParseBreakStatement() {
    assert(m_token == TokenType::Break);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;

    if (ConsumeIf(TokenType::If)) {
        Result(error, expr) = ParseExpression();
    }

    ExpectEOL(error);

    return MakeResult<BreakStatement>(error, pos, expr);
}

StmtResult Parser::ParseContinueStatement() {
    assert(m_token == TokenType::Continue);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;

    if (ConsumeIf(TokenType::If)) {
        Result(error, expr) = ParseExpression();
    }

    ExpectEOL(error);
    
    return MakeResult<ContinueStatement>(error, pos, expr);
}

StmtResult Parser::ParseReturnStatement() {
    assert(m_token == TokenType::Return);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;

    if (!m_newline) {
        Result(error, expr) = ParseExpression();
    }

    ExpectEOL(error);

    return MakeResult<ReturnStatement>(error, pos, expr);
}

StmtResult Parser::ParseEmptyStatement() {
    assert(m_token == TokenType::Pass);

    auto pos = ConsumeToken();
    auto error = false;

    ExpectEOL(error);

    return MakeResult<EmptyStatement>(error, pos);
}

StmtResult Parser::ParseAssertStatement() {
    assert(m_token == TokenType::Assert);

    auto pos = ConsumeToken();
    auto error = false;
    Expression* expr;
    Expression* msg;

    Result(error, expr) = ParseBooleanExpression();

    if (ConsumeIf(TokenType::Colon)) {
        Result(error, msg) = ParseExpression();
    }

    ExpectEOL(error);
    
    return MakeResult<AssertStatement>(error, pos, expr, msg);
}

/*
 * variable_declaration
 *     : 'var' var_name ('=' expression)? EOL
 *     ;
 */
StmtResult Parser::ParseVariableDeclaration() {
    assert(m_token == TokenType::Var);

    auto pos = ConsumeToken();
    auto error = false;
    StringRef id;
    Expression* value = nullptr;

    id = ExpectIdentifier(error);

    if (ConsumeIf(TokenType::Assign))
        Result(error, value) = ParseExpression();

    ExpectEOL(error, SkipToEOL);

    return MakeResult<VariableDeclaration>(error, pos, id, value);
}