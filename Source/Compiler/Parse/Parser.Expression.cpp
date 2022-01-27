/*
 * Parser.Expression.cpp
 * - Syntax analyzer for BuildScript language.
 *   This file contains partial code of parser that parses expression.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Parser.h>

#include <tuple>

#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/AST/Statements.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/ErrorReporter.ReportID.h>
#include <BuildScript/Compiler/SourceText.h>
#include <BuildScript/Utils/Convert.h>

using namespace BuildScript;

static constexpr auto MAX_RANK = 9;

static std::tuple<int, BinaryOp> GetRank(TokenType type) {
    switch (type) {
        default:
            return { -1, BinaryOp::Mul /* This is a placeholder and do not use as valid value. */ };

        // multiplicative_expression
        case TokenType::Mul:
            return { 9, BinaryOp::Mul };
        case TokenType::Div:
            return { 9, BinaryOp::Div };
        case TokenType::Mod:
            return { 9, BinaryOp::Mod };

        // additive_expression
        case TokenType::Add:
            return { 8, BinaryOp::Add };
        case TokenType::Sub:
            return { 8, BinaryOp::Sub };

        // shift_expression
        case TokenType::LeftShift:
            return { 7, BinaryOp::LeftShift };
        case TokenType::RightShift:
            return { 7, BinaryOp::RightShift };

        // binary_and_expression
        case TokenType::BitAnd:
            return { 6, BinaryOp::BitAnd };

        // binary_xor_expression
        case TokenType::BitXor:
            return { 5, BinaryOp::BitXor };

        // binary_or_expression
        case TokenType::BitOr:
            return { 4, BinaryOp::BitOr };

        // relative_expression
        case TokenType::Less:
            return { 3, BinaryOp::Less };
        case TokenType::LessOrEqual:
            return { 3, BinaryOp::LessOrEqual };
        case TokenType::Grater:
            return { 3, BinaryOp::Grater };
        case TokenType::GraterOrEqual:
            return { 3, BinaryOp::GraterOrEqual };

        // equality_expression
        case TokenType::Equal:
            return { 2, BinaryOp::Equal };
        case TokenType::NotEqual:
            return { 2, BinaryOp::NotEqual };

        // logical_and_expression
        case TokenType::And:
            return { 1, BinaryOp::LogicalAnd };

        // logical_or_expression
        case TokenType::Or:
            return { 0, BinaryOp::LogicalOr };
    }
}

/*
 * ternary_expression
 *  : binary_expression 'if' boolean_expression 'else' ternary_expression
 *  ;
 */
Expression* Parser::ParseTernaryExpression() {
    auto* expr = ParseBinaryExpression();
    SourcePosition _if;

    // To distinguish ternary expression AND expression and if statement,
    // check newline before consume 'if'.
    if (!HasEOL() && ConsumeIf(TokenType::If, _if)) {
        auto* cond = ParseBooleanExpression();
        auto _else = RequireToken(TokenType::Else);
        auto* valueF = ParseTernaryExpression();

        expr = TernaryExpression::Create(m_context, expr, _if, cond, _else, valueF);
    }

    return expr;
}

/*
 * binary_expression
 *  : logical_or_expression
 *  ;
 *
 * boolean_expression
 *  : logical_or_expression
 *  ;
 *
 * logical_or_expression
 *  : logical_and_expression
 *  | logical_or_expression 'or' logical_and_expression
 *  ;
 *
 * logical_and_expression
 *  : equality_expression
 *  | logical_and_expression 'and' equality_expression
 *  ;
 *
 * equality_expression
 *  : relative_expression
 *  | equality_expression '==' relative_expression
 *  | equality_expression '!=' relative_expression
 *  ;
 *
 * relative_expression
 *  : binary_or_expression
 *  | relative_expression '<' binary_or_expression
 *  | relative_expression '<=' binary_or_expression
 *  | relative_expression '>' binary_or_expression
 *  | relative_expression '>=' binary_or_expression
 *  | type_test_expression
 *  | containment_test_expression
 *  ;
 *
 * type_test_expression
 *  | relative_expression 'is' ('not')? identifier
 *  ;
 *
 * containment_test_expression
 *  | relative_expression ('not')? 'in' postfix_expression
 *  ;
 *
 * binary_or_expression
 *  : binary_xor_expression
 *  | binary_or_expression '|' binary_xor_expression
 *  ;
 *
 * binary_xor_expression
 *  : binary_and_expression
 *  | binary_xor_expression '^' binary_and_expression
 *  ;
 *
 * binary_and_expression
 *  : shift_expression
 *  | binary_and_expression '&' shift_expression
 *  ;
 *
 * shift_expression
 *  : additive_expression
 *  | shift_expression '<<' additive_expression
 *  | shift_expression '>>' additive_expression
 *  ;
 *
 * additive_expression
 *  : multiplicative_expression
 *  | additive_expression '+' multiplicative_expression
 *  | additive_expression '-' multiplicative_expression
 *  ;
 *
 * multiplicative_expression
 *  : unary_expression
 *  | multiplicative_expression '*' unary_expression
 *  | multiplicative_expression '/' unary_expression
 *  | multiplicative_expression '%' unary_expression
 *  ;
 */
Expression* Parser::ParseBinaryExpression(int rank) {
    auto* left = (rank < MAX_RANK) ? ParseBinaryExpression(rank + 1) : ParseUnaryExpression();

    auto [_rank, op] = GetRank(m_token.Type);

    if (_rank < rank) {
        std::array<SourcePosition, 2> pos;
        bool negate;

        // test if the expression is type test expression
        if (m_token == TokenType::Is) {
            pos[0] = ConsumeToken();
            negate = ConsumeIf(TokenType::Not, pos[1]);
            auto type = RequireIdentifier();

            left = TypeTestExpression::Create(m_context, left, pos, negate, std::move(type));
        }
        // test if the expression is containment test expression
        else if (m_token == TokenType::Not || m_token == TokenType::In) {
            negate = (m_token == TokenType::Not);
            pos[0] = ConsumeToken();

            if (negate) { pos[1] = RequireToken(TokenType::In); }

            auto* target = ParsePostfixExpression();

            left = ContainmentTestExpression::Create(m_context, left, pos, negate, target);
        }
        // Otherwise, end of binary expression is reached.

        return left;
    }

    auto pos = ConsumeToken();
    auto* right = ParseBinaryExpression(rank);

    return BinaryExpression::Create(m_context, left, op, pos, right);
}

/*
 * unary_expression
 *  : postfix_expression
 *  | unary_op unary_expression
 *  | defined_expression
 *  | raise_expression
 *  ;
 *
 * unary_op
 *  : '+'
 *  | '-'
 *  | '~'
 *  | 'not'
 *  ;
 */

Expression* Parser::ParseUnaryExpression() {
    UnaryOp op;

    switch (m_token.Type) {
        case TokenType::Add:
            op = UnaryOp::Identity;
            break;

        case TokenType::Sub:
            op = UnaryOp::Negate;
            break;

        case TokenType::BitNot:
            op = UnaryOp::BinaryNot;
            break;

        case TokenType::Not:
            op = UnaryOp::LogicalNot;
            break;

        case TokenType::Defined:
            return ParseDefinedExpression();

        case TokenType::Raise:
            return ParseRaiseExpression();

        default:
            return ParsePostfixExpression();
    }

    auto pos = ConsumeToken();
    auto* expr = ParseUnaryExpression();

    return UnaryExpression::Create(m_context, op, pos, expr);
}

/*
 * defined_expression
 *  : 'defined' identifier ('in' postfix_expression)?
 *  ;
 */
Expression* Parser::ParseDefinedExpression() {
    assert(m_token == TokenType::Defined);

    auto defined = ConsumeToken();
    auto id = RequireIdentifier();
    SourcePosition in;
    Expression* expr = nullptr;

    if (ConsumeIf(TokenType::In, in)) {
        expr = ParsePostfixExpression();
    }

    return DefinedExpression::Create(m_context, defined, std::move(id), in, expr);
}

/*
 * raise_expression
 *  : 'raise' postfix_expression
 *  ;
 */
Expression* Parser::ParseRaiseExpression() {
    assert(m_token == TokenType::Raise);

    auto raise = ConsumeToken();
    auto* expr = ParsePostfixExpression();

    return RaiseExpression::Create(m_context, raise, expr);
}

/*
 * postfix_expression
 *  : primary_expression
 *  | postfix_expression '.' identifier
 *  | postfix_expression '(' expression_list? ')'
 *  | postfix_expression '[' expression ']'
 *  ;
 *
 * expression_list
 *  : expression
 *  | expression_list ',' expression
 *  ;
 */
Expression* Parser::ParsePostfixExpression() {
    auto* expr = ParsePrimaryExpression();

    while (true) {
        switch (m_token.Type) {
            case TokenType::Dot: {
                auto dot = ConsumeToken();
                auto member = RequireIdentifier();
                expr = MemberAccessExpression::Create(m_context, expr, dot, std::move(member));
                break;
            }

            case TokenType::LeftParen: {
                std::vector<Expression*> items;
                std::vector<SourcePosition> commas;
                auto open = ConsumeToken();

                if (m_token != TokenType::RightParen) {
                    ParseExpressionList(items, commas);
                }

                auto close = RequireToken(TokenType::RightParen);

                expr = InvocationExpression::Create(m_context, expr, open, items, commas, close);
                break;
            }

            case TokenType::LeftSquare: {
                auto open = ConsumeToken();
                auto* index = ParseExpression();
                auto close = RequireToken(TokenType::RightSquare);

                expr = SubscriptExpression::Create(m_context, expr, open, index, close);
                break;
            }

            default:
                return expr;
        }
    }
}

/*
 * primary_expression
 *  : identifier
 *  | literals
 *  | '(' expression ')'
 *  | list
 *  | map
 *  | closure
 *  ;
 */
Expression* Parser::ParsePrimaryExpression() {
    switch (m_token.Type) {
        case TokenType::String:
            return ParseString();

        case TokenType::LeftParen:
            return CheckForClosure() ? ParseClosure() : ParseParenthesis();

        case TokenType::LeftBrace:
            return ParseMap();

        case TokenType::LeftSquare:
            return ParseList();

        case TokenType::Inputs:
        case TokenType::Outputs:
        case TokenType::From:
        case TokenType::Do:
        case TokenType::DoFirst:
        case TokenType::DoLast:
        case TokenType::DependsOn:
        case TokenType::Identifier: {
            auto name = RequireIdentifier();
            return LiteralExpression::CreateVariable(m_context, name);
        }

        case TokenType::Integer: {
            auto range = ConsumeTokenRange();
            auto [error, value] = ConvertInteger(m_source.GetString(range));

            if (error) {
                m_reporter.Report(m_token.GetPosition(), ReportID::ParseOverflowInteger);
                return InvalidExpression::Create(m_context, range);
            }
            else {
                return LiteralExpression::CreateInteger(m_context, range, value);
            }
        }

        case TokenType::Float: {
            auto range = ConsumeTokenRange();
            auto [error, value] = ConvertFloat(m_source.GetString(range));

            if (error) {
                auto id = isinf(value) ? ReportID::ParseOverflowFloat : ReportID::ParseUnderflowFloat;

                m_reporter.Report(m_token.GetPosition(), id);

                return InvalidExpression::Create(m_context, range);
            }
            else {
                return LiteralExpression::CreateFloat(m_context, range, value);
            }
        }

        case TokenType::True:
        case TokenType::False: {
            auto value = (m_token == TokenType::True);
            auto range = ConsumeTokenRange();

            return LiteralExpression::CreateBoolean(m_context, range, value);
        }

        case TokenType::None:
            return LiteralExpression::CreateNone(m_context, ConsumeTokenRange());

        case TokenType::Self:
            return LiteralExpression::CreateSelf(m_context, ConsumeTokenRange());

        case TokenType::Super:
            return LiteralExpression::CreateSuper(m_context, ConsumeTokenRange());

        default:
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectExpression);
            return InvalidExpression::Create(m_context, SkipTokenExpr());
    }
}

Expression* Parser::ParseParenthesis() {
    assert(m_token == TokenType::LeftParen);

    auto open = ConsumeToken();
    auto* expr = ParseExpression();
    auto close = RequireToken(TokenType::RightParen);

    return ParenthesizedExpression::Create(m_context, open, expr, close);
}

Expression* Parser::ParseString() {
    assert(m_token == TokenType::String);

    auto image = m_token.Image;
    auto stringRange = ConsumeTokenRange();
    auto interpolations = m_lexer.ScanInterpolations(stringRange);
    std::vector<Expression*> exprs;

    for (const auto& range : interpolations) {
        SourceText partial(m_source, range);
        Parser parser(partial, m_reporter, m_context);

        auto* expr = parser.ParseExpression();
        exprs.push_back(expr);
    }

    return LiteralExpression::CreateString(m_context, stringRange, std::move(image), exprs);
}

/*
 * list
 *  : '[' expression_list? ']'
 *  ;
 */
Expression* Parser::ParseList() {
    assert(m_token == TokenType::LeftSquare);

    auto open = ConsumeToken();
    std::vector<Expression*> items;
    std::vector<SourcePosition> commas;

    if (m_token != TokenType::RightSquare) {
        ParseExpressionList(items, commas);
    }

    auto close = RequireToken(TokenType::RightSquare);

    return ListExpression::Create(m_context, open, items, commas, close);
}

/*
 * map
 *  : '{' key_value_list? '}'
 *  ;
 *
 * key_value_list
 *  : key_value
 *  | key_value_list ',' key_value
 *  ;
 *
 * key_value
 *  : expression ':' expression
 *  ;
 */
Expression* Parser::ParseMap() {
    assert(m_token == TokenType::LeftBrace);

    auto open = ConsumeToken();
    std::vector<Expression*> items;
    std::vector<SourcePosition> commas;

    if (m_token != TokenType::RightBrace) {
        bool hasComma;

        do {
            auto* key = ParseConstantExpression();
            auto colon = RequireToken(TokenType::Colon);
            auto* value = ParseExpression();
            SourcePosition comma;

            hasComma = ConsumeIf(TokenType::Comma, comma);

            items.push_back(KeyValuePair::Create(m_context, key, colon, value));
            if (hasComma) {
                commas.push_back(comma);
            }
        } while (hasComma);
    }

    auto close = RequireToken(TokenType::RightBrace);

    return MapExpression::Create(m_context, open, items, commas, close);
}

/*
 * closure
 *  : parameters '=>' expression
 *  | parameters '=>' block
 *  | parameters '=>' 'pass'
 *  ;
 */
Expression* Parser::ParseClosure() {
    assert(m_token == TokenType::LeftParen);

    auto* params = ParseParameters();
    auto arrow = RequireToken(TokenType::Arrow);
    Statement* node;

    if (m_token != TokenType::LeftBrace) {
        Expression* body;

        if (m_token == TokenType::Pass) {
            body = PassExpression::Create(m_context, ConsumeToken());
        }
        else {
            body = ParseExpression();
        }

        RequireEOL();

        node = ArrowStatement::Create(m_context, arrow, body);
    }
    else {
        node = ParseBody();
    }

    return ClosureExpression::Create(m_context, params, arrow, node);
}