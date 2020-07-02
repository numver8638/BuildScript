/*
 * Parser.Expression.cpp
 * - Syntax analyzer (a.k.a parser) for BuildScript language.
 *   This file contains part of Parser that parse expressions.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parser.h>
#include <BuildScript/Compiler/Parser.Shared.h>

#include <BuildScript/Compiler/AST/Expressions.h>

using namespace BuildScript;

template <typename T, typename... Args>
ExprResult MakeResult(bool error, Args&& ... args) {
    T* expr = new T(std::forward<Args>(args)...);
    return ExprResult(error, expr);
}

/*
 * ternary_expression
 *     : binary_expression
 *     | ternary_expression 'if' boolean_expression 'else' binary_expression
 *     ;
 */
ExprResult Parser::ParseTernaryExpression() {
    auto expr = ParseBinaryExpression();

    if (ConsumeIf(TokenType::If)) {
        auto error = false;
        ExprResult condition, elseVal;

        condition = ParseBooleanExpression();

        Expect(error, TokenType::Else, [&](const SourcePosition& pos) {
            // Do not report error if condition.HasError() is true.
            // This error may be caused by previous error.
            if (!condition.HasError()) {
                m_reporter.Report(pos, ReportID::ParseExpectElse);
            }
        });

        if (!condition.HasError())
            elseVal = ParseTernaryExpression();

        error |= MakeStatus(expr, condition, elseVal);
        expr = MakeResult<TernaryExpression>(error, expr.GetValue(), condition.GetValue(), elseVal.GetValue());
    }

    return expr;
}

static std::tuple<int, BinaryOp> GetRank(TokenType type) {
    switch (type) {
        default:
            return {-1, BinaryOp::Mul/* This is a placeholder and do not use as valid value. */ };

            // multiplicative_expression
        case TokenType::Mul:
            return {9, BinaryOp::Mul};
        case TokenType::Div:
            return {9, BinaryOp::Div};
        case TokenType::Mod:
            return {9, BinaryOp::Mod};

            // additive_expression
        case TokenType::Add:
            return {8, BinaryOp::Add};
        case TokenType::Sub:
            return {8, BinaryOp::Sub};

            // shift_expression
        case TokenType::LeftShift:
            return {7, BinaryOp::LeftShift};
        case TokenType::RightShift:
            return {7, BinaryOp::RightShift};

            // binary_and_expression
        case TokenType::BitAnd:
            return {6, BinaryOp::BitAnd};

            // binary_xor_expression
        case TokenType::BitXor:
            return {5, BinaryOp::BitXor};

            // binary_or_expression
        case TokenType::BitOr:
            return {4, BinaryOp::BitOr};

            // relative_expression
        case TokenType::Less:
            return {3, BinaryOp::Less};
        case TokenType::LessOrEqual:
            return {3, BinaryOp::LessOrEqual};
        case TokenType::Grater:
            return {3, BinaryOp::Grater};
        case TokenType::GraterOrEqual:
            return {3, BinaryOp::GraterOrEqual};
        case TokenType::Is:
            return {3, BinaryOp::Is};
        case TokenType::In:
            return {3, BinaryOp::In};
        case TokenType::Not:
            return {3, BinaryOp::NotIn};

            // equality_expression
        case TokenType::Equal:
            return {2, BinaryOp::Equal};
        case TokenType::NotEqual:
            return {2, BinaryOp::NotEqual};

            // logical_and_expression
        case TokenType::And:
            return {1, BinaryOp::LogicalAnd};

            // logical_or_expression
        case TokenType::Or:
            return {0, BinaryOp::LogicalOr};
    }
}

/*
 * logical_or_expression
 *     : logical_and_expression
 *     | logical_or_expression 'or' logical_and_expression
 *     ;
 * 
 * logical_and_expression
 *     : equality_expression
 *     | logical_and_expression 'and' equality_expression
 *     ;
 * 
 * equality_expression
 *     : relational_expression
 *     | equality_expression '==' relational_expression
 *     | equality_expression '!=' relational_expression
 *     ;
 * 
 * relational_expression
 *     : bitwise_or_expression
 *     | relational_expression relational_op bitwise_or_expression
 *     | relational_expression 'is' ('not')? identifier
 *     | relational_expression ('not')? 'in' postfix_expression
 *     ;
 * 
 * relational_op
 *     : '<'
 *     | '>'
 *     | '<='
 *     | '>='
 *     ;
 * 
 * bitwise_or_expression
 *     : bitwise_xor_expression
 *     | bitwise_or_expression '|' bitwise_xor_expression
 *     ;
 * 
 * bitwise_xor_expression
 *     : bitwise_and_expression
 *     | bitwise_xor_expression '^' bitwise_and_expression
 *     ;
 * 
 * bitwise_and_expression
 *     : shift_expression
 *     | bitwise_and_expression '&' shift_expression
 *     ;
 * 
 * shift_expression
 *     : additive_expression
 *     | shift_expression '<<' additive_expression
 *     | shift_expression '>>' additive_expression
 *     ;
 * 
 * additive_expression
 *     : multiplicative_expression
 *     | additive_expression '+' multiplicative_expression
 *     | additive_expression '-' multiplicative_expression
 *     ;
 * 
 * multiplicative_expression
 *     : unary_expression
 *     | multiplicative_expression '*' unary_expression
 *     | multiplicative_expression '/' unary_expression
 *     | multiplicative_expression '%' unary_expression
 *     ;
 */
ExprResult Parser::ParseBinaryExpression() {
    using T = std::tuple<
        ExprResult,     // EXPR
        BinaryOp,       // OP
        int             // RANK
    >;

    constexpr int RANK = 2;

    auto left = ParseUnaryExpression();
    std::vector<T> stack;
    bool loop = true;

    do {
        int rank;
        BinaryOp op;

        std::tie(rank, op) = GetRank(m_token.Type);

        if (rank != -1) {
            ConsumeToken();

            // Extra process for `is not` and `not in`
            if (op == BinaryOp::Is && ConsumeIf(TokenType::Not)) {
                op = BinaryOp::IsNot;
            }

            if (op == BinaryOp::NotIn) {
                bool _; // placeholder

                Expect(_, TokenType::In, [&](const SourcePosition& pos) {
                    m_reporter.Report(pos, ReportID::ParseExpectIn);
                    left.Taint();
                });
            }

            auto right = ParseUnaryExpression();

            if (stack.empty() || std::get<RANK>(stack.back()) <= rank) {
                stack.emplace_back(left, op, rank);
                left = right;
            }
        } else
            loop = false;

        while (!stack.empty() && std::get<RANK>(stack.back()) > rank) {
            ExprResult expr;
            BinaryOp op;
            int _;

            std::tie(expr, op, _) = stack.back();
            stack.pop_back();

            auto error = MakeStatus(left, expr);
            left = MakeResult<BinaryExpression>(error, expr.GetValue(), left.GetValue(), op);
        }
    } while (loop);

    return left;
}

/*
 * unary_expression
 *     : cast_expression
 *     | unary_op unary_expression
 *     | 'defined' identifier ('in' cast_expression)?
 *     ;
 * 
 * unary_op
 *     : '+'
 *     | '-'
 *     | '~'
 *     | 'not'
 *     ;
 */
ExprResult Parser::ParseUnaryExpression() {
    using T = std::tuple<
        UnaryOp,       // OP
        SourcePosition // POS
    >;

    ExprResult expr;
    std::vector<T> stack;

    do {
        switch (m_token.Type) {
            default:
                expr = ParseCastExpression();
                break;

            case TokenType::Not:
                stack.emplace_back(UnaryOp::LogicalNot, ConsumeToken());
                break;

            case TokenType::Add:
                stack.emplace_back(UnaryOp::Identity, ConsumeToken());
                break;

            case TokenType::Sub:
                stack.emplace_back(UnaryOp::Negate, ConsumeToken());
                break;

            case TokenType::BitNot:
                stack.emplace_back(UnaryOp::BinaryNot, ConsumeToken());
                break;

            case TokenType::Defined: {
                auto error = false;
                SourcePosition pos = ConsumeToken();
                StringRef id = ExpectIdentifier(error);
                Expression* target;

                if (ConsumeIf(TokenType::In)) {
                    Result(error, target) = ParseCastExpression();
                }

                expr = MakeResult<DefinedExpression>(error, pos, id, target);
                break;
            }
        }
    } while (!expr.HasValue());

    while (!stack.empty()) {
        UnaryOp op;
        SourcePosition pos;

        std::tie(op, pos) = stack.back();
        stack.pop_back();

        expr = MakeResult<UnaryExpression>(MakeStatus(expr), pos, expr.GetValue(), op);
    }

    return expr;
}

/*
 * cast_expression
 *     : postfix_expression
 *     | cast_expression 'as' typename
 *     ;
 * 
 * typename
 *     : identifier
 *     ;
 */
ExprResult Parser::ParseCastExpression() {
    auto expr = ParsePostfixExpression();

    if (ConsumeIf(TokenType::As)) {
        auto error = false;
        auto type = ExpectIdentifier(error);
        expr = MakeResult<CastExpression>(MakeStatus(error, expr), expr.GetValue(), type);
    }

    return expr;
}

/*
 * postfix_expression
 *     : primary_expression
 *     | postfix_expression '.' identifier
 *     | postfix_expression '[' expression ']'
 *     | postfix_expression '(' ')'
 *     ;
 */
ExprResult Parser::ParsePostfixExpression() {
    auto expr = ParsePrimaryExpression();

    while (true) {
        switch (m_token.Type) {
            case TokenType::LeftParen: {
                ConsumeToken(); // consume (

                auto error = false;
                std::vector<Expression*> params;

                if (m_token != TokenType::RightParen) {
                    do {
                        Expression* param;
                        Result(error, param) = ParseExpression();
                        params.push_back(param);
                    } while (ConsumeIf(TokenType::Comma));
                }

                Expect(error, TokenType::RightParen, ExpectParen);

                expr = MakeResult<InvocationExpression>(MakeStatus(error, expr), expr.GetValue(), std::move(params));
                break;
            }

            case TokenType::LeftSquare: {
                ConsumeToken(); // consume [
                auto error = false;
                Expression* index;

                Result(error, index) = ParseExpression();
                Expect(error, TokenType::RightSquare, ExpectSquare);

                expr = MakeResult<SubscriptExpression>(MakeStatus(error, expr), expr.GetValue(), index);
                break;
            }

            case TokenType::Dot: {
                ConsumeToken(); // consume .
                auto error = false;
                auto member = ExpectIdentifier(error);
                expr = MakeResult<MemberAccessExpression>(MakeStatus(error, expr), expr.GetValue(), member);
                break;
            }

            default:
                return expr;
        }
    }
}

/* 
 * primary_expression
 *     : variable
 *     | literal
 *     | list
 *     | map
 *     | tuple
 *     | closure
 *     | '(' expression ')'
 *     ;
 * 
 * variable
 *     : identifier
 *     ;
 * 
 * list
 *     : '[' expression_list? ']'
 *     ;
 * 
 * map
 *     : '{' kv_pairs? '}'
 *     ;
 * 
 * kv_pairs
 *     : kv_pair
 *     | kv_pairs ',' kv_pair
 *     ;
 * 
 * kv_pair
 *     : key ':' expression
 *     ;
 * 
 * key
 *     : identifier
 *     | constant
 *     ;
 * 
 * constant
 *     : <an expression which can be evaluated in compile time>
 *     ;
 * 
 * tuple
 *     : unnamed_tuple
 *     | named_tuple
 *     ;
 * 
 * unnamed_tuple
 *     | '(' expression_list ')'
 *     ;
 * 
 * named_tuple
 *     : '(' kv_pairs ')'
 *     ;
 */
ExprResult Parser::ParsePrimaryExpression() {
    Expression* expr;
    auto error = false;
    LiteralKind kind;

    switch (m_token.Type) {
        case TokenType::Inputs:
        case TokenType::Outputs:
        case TokenType::DependsOn:
        case TokenType::Do:
        case TokenType::DoFirst:
        case TokenType::DoLast:
        case TokenType::From:
        case TokenType::Task:
        case TokenType::With:
        case TokenType::Identifier:
            kind = LiteralKind::Identifier;
            goto LiteralExpr;

        case TokenType::Integer:
            kind = LiteralKind::Integer;
            goto LiteralExpr;

        case TokenType::Float:
            kind = LiteralKind::Float;
            goto LiteralExpr;

        case TokenType::True:
            kind = LiteralKind::True;
            goto LiteralExpr;

        case TokenType::False:
            kind = LiteralKind::False;
            goto LiteralExpr;

        case TokenType::None:
            kind = LiteralKind::None;
            goto LiteralExpr;

        case TokenType::Self:
            kind = LiteralKind::Self;
            goto LiteralExpr;

        case TokenType::Super:
            kind = LiteralKind::Super;
            goto LiteralExpr;

        LiteralExpr:
            expr = new LiteralExpression(m_token.Position, kind, m_token.Image);
            break;

        case TokenType::LeftParen:
            return ParseParenthesis();

        case TokenType::String:
            return ParseString();

        case TokenType::LeftBrace:
            return ParseMap();

        case TokenType::LeftSquare:
            return ParseList();

        case TokenType::EndOfFile:
            m_reporter.Report(m_token.Position, ReportID::ParseUnexpectedEOF);
            goto HandleError;

        default:
            m_reporter.Report(m_token.Position, ReportID::ParseExpectExpression);
            goto HandleError;

        case TokenType::Invalid:
            // Do not report error because already reported at lexer.
            /* [[fallthrough]] */

        HandleError:
            expr = new ErrorExpression(m_token.Position);
            error = true;
            break;
    }

    ConsumeToken();

    return ExprResult(error, expr);
}

ExprResult Parser::ParseParenthesis() {
    assert(m_token == TokenType::LeftParen);

    auto pos = ConsumeToken();
    auto error = false;
    std::vector<Expression*> args;
    Expression* expr;

    // Check more than one expression contains in parenthesis.
    if (!ConsumeIf(TokenType::RightParen)) {
        auto key = ParseExpression();

        // This is NamedTuple if it has colon after expression.
        if (ConsumeIf(TokenType::Colon))
            return ParseNamedTuple(pos, key);

        Result(error, expr) = key;
        args.push_back(expr);

        while (ConsumeIf(TokenType::Comma)) {
            Result(error, expr) = ParseExpression();
            args.push_back(expr);
        }

        Expect(error, TokenType::RightParen, ExpectParen);
    }

    // Parse parenthesis as closure parameter list if '=>' is appeared after parenthesis.
    if (ConsumeIf(TokenType::Arrow)) {
        return ParseClosure(error, pos, args);
    } else
        // If exactly one expression is found in parenthesis, it is parenthesized expression.
        // If not, it is Tuple.
        return args.size() == 1 ? ExprResult(error, expr) : MakeResult<TupleExpression>(error, pos, std::move(args));
}

/*
 * string_literal
 *     : '"' character* '"'
 *     | '\'' character* '\''
 *     ;
 *
 * character
 *     : <any unicode character except '\\', '$', '"', '\'', '\r', '\n' and EOF>
 *     | escape_sequence
 *     | interpolated_expression
 *     ;
 *
 * escape_sequence
 *     : '\\a' | '\\b' | '\\f' | '\\n' | '\\r' | '\\t' | '\\v'
 *     | '\\\'' | '\\"' | '\\$' | '\\\\'
 *     | '\\u' hexadecimal_digit{4}
 *     | '\\U' hexadecimal_digit{8}
 *     | '\\x' hexadecimal_digit{2}
 *     | '\\' octal_digit{1, 3}
 *     ;
 *
 * interpolated_expression
 *     : '$' identifier
 *     | '${' expression '}'
 *     ;
 */
ExprResult Parser::ParseString() {
    assert(m_token == TokenType::String);

    auto pos = m_token.Position;
    std::string str;
    char buffer[32];
    int index = 0;
    std::vector<Expression*> exprs;
    auto error = false;

    str.append(m_token.Image);
    ConsumeToken();

    while (true) {
        if (m_token == TokenType::Invalid) {
            error = true;
            SkipUntil(TokenType::InterpolateEnd, TokenType::String);
        } else if (ConsumeIf(TokenType::InterpolateBegin)) {
            Expression* expr;
            Result(error, expr) = ParseExpression();

            Expect(error, TokenType::InterpolateEnd, [&](const SourcePosition& pos) {
                m_reporter.Report(pos, ReportID::ParseExpectInterpolateEnd);
                SkipUntil(TokenType::InterpolateEnd);
            });

            exprs.push_back(expr);
        } else if (m_token == TokenType::InterpolatedVar) {
            StringRef id = m_token.Image;
            auto pos = ConsumeToken();
            exprs.push_back(new LiteralExpression(pos, LiteralKind::Identifier, id));
        } else
            break;

        if (index > 255) {
            m_reporter.Report(exprs.back()->GetPosition(), ReportID::ParseMaxInterpolatedExpressionReached);
        } else {
            auto length = std::snprintf(buffer, sizeof(buffer), "{%d}", index++);
            str.append(buffer, length);
        }

        if (m_token != TokenType::String) {
            m_reporter.Report(m_token.Position, ReportID::ParseUnexpectedEOL);
            error = true;
        } else {
            str.append(m_token.Image);
            ConsumeToken();
        }
    }

    return MakeResult<StringLiteral>(error, pos, std::move(str), std::move(exprs));
}

/*
 * map
 *     : '{' kv_pairs? '}'
 *     ;
 * 
 * kv_pairs
 *     : kv_pair
 *     | kv_pairs ',' kv_pair
 *     ;
 * 
 * kv_pair
 *     : key ':' expression
 *     ;
 * 
 * key
 *     : identifier
 *     | constant
 *     ;
 * 
 * constant
 *     : <an expression which can be evaluated in compile time>
 *     ;
 */
ExprResult Parser::ParseMap() {
    assert(m_token == TokenType::LeftBrace);

    auto begin = ConsumeToken();
    std::map<Expression*, Expression*> items;
    bool error = false;

    if (m_token != TokenType::RightBrace) {
        do {
            ExprResult key, value;

            key = ParseExpression();
            Expect(error, TokenType::Colon, ExpectColon);
            value = ParseExpression();

            if (key.HasError() && value.HasError())
                SkipUntil(TokenType::Comma, TokenType::RightBrace);

            error |= MakeStatus(key, value);
            items.emplace(key.GetValue(), value.GetValue());
        } while (ConsumeIf(TokenType::Comma));
    }

    Expect(error, TokenType::RightBrace, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseBraceNotClosed);
    });

    return MakeResult<MapExpression>(error, begin, std::move(items));
}

/*
 * list
 *    : '[' expression_list? ']'
 *    ;
 */
ExprResult Parser::ParseList() {
    assert(m_token == TokenType::LeftSquare);

    auto begin = ConsumeToken();
    std::vector<Expression*> items;
    bool error = false;

    if (m_token != TokenType::RightSquare) {
        do {
            Expression* item;
            Result(error, item) = ParseExpression();
            items.push_back(item);
        } while (ConsumeIf(TokenType::Comma));

        Expect(error, TokenType::RightSquare, ExpectSquare);
    } else
        ConsumeToken(); // consume ]

    return MakeResult<ListExpression>(error, begin, std::move(items));
}

/* 
 * closure
 *     : '(' parameter_list? ')' '=>' block
 *     | '(' parameter_list? ')' '=>' expression
 *     ;
 */
ExprResult Parser::ParseClosure(bool error, const SourcePosition& pos, std::vector<Expression*>& exprs) {
    ASTNode* body = nullptr;
    std::vector<ClosureExpression::Parameter> params;

    // Convert Expression* to StringRef
    for (auto e : exprs) {
        if (e->GetKind() != ExpressionKind::Literal ||
            static_cast<LiteralExpression*>(e)->GetLiteralKind() != LiteralKind::Identifier) {
            m_reporter.Report(e->GetPosition(), ReportID::ParseExpectIdentifier);
            error = true;
        } else
            params.emplace_back(static_cast<LiteralExpression*>(e)->GetImage(), e->GetPosition());

        delete e;
    }

    if (m_token == TokenType::LeftBrace)
        Result(error, body) = ParseBlock();
    else
        Result(error, body) = ParseExpression();

    return MakeResult<ClosureExpression>(error, pos, std::move(params), body);
}

/*
 * named_tuple
 *     : '(' kv_pairs ')'
 *     ;
 */
ExprResult Parser::ParseNamedTuple(const SourcePosition& pos, ExprResult key) {
    std::map<Expression*, Expression*> items;

    auto error = false;
    auto value = ParseExpression();

    items.emplace(key.GetValue(), value.GetValue());
    error |= MakeStatus(key, value);

    while (ConsumeIf(TokenType::Comma)) {
        key = ParseExpression();
        Expect(error, TokenType::Colon, ExpectColon);
        value = ParseExpression();

        error |= MakeStatus(key, value);
        items.emplace(key.GetValue(), value.GetValue());
    }

    Expect(error, TokenType::RightSquare, ExpectSquare);

    return MakeResult<NamedTupleExpression>(error, pos, std::move(items));
}