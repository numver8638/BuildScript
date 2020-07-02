/*
 * Parser.ClassMember.cpp
 * - Syntax analyzer (a.k.a parser) for BuildScript language.
 *   This file contains part of Parser that parse class members.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parser.h>
#include <BuildScript/Compiler/Parser.Shared.h>

#include <BuildScript/Compiler/AST/Declarations.h>

using namespace BuildScript;

template <typename T, typename... Args>
ParseResult<ClassMember> MakeResult(bool status, Args&&... args) {
    T* decl = new T(std::forward<Args>(args)...);
    return ParseResult<ClassMember>(status, decl);
}

/*
 * class_static_member
 *     : 'static' identifier '=' expression EOL
 *     ;
 */
ParseResult<ClassMember> Parser::ParseClassStatic() {
    assert(m_token == TokenType::Static);

    auto pos = ConsumeToken();
    auto error = false;

    if (m_token == TokenType::Def) {
        return ParseClassMethod(pos, true);
    }

    auto id = ExpectIdentifier(error);
    Expression* value;

    Expect(error, TokenType::Assign, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseExpectAssign);
    });
    Result(error, value) = ParseExpression();
    ExpectEOL(error);

    return MakeResult<ClassStaticMember>(error, pos, id, value);
}

/*
 * class_initializer
 *     : 'init' parameters block
 *     ;
 */
ParseResult<ClassMember> Parser::ParseClassInit() {
    assert(m_token == TokenType::Init);

    auto pos = ConsumeToken();
    auto error = false;
    Parameters* params;
    Statement* body;

    Result(error, params) = ParseParameters();
    Result(error, body) = ParseBlock();

    return MakeResult<ClassInit>(error, pos, params, body);
}

/*
 * class_deinitializer
 *     : 'deinit' block
 *     ;
 */
ParseResult<ClassMember> Parser::ParseClassDeinit() {
    assert(m_token == TokenType::Deinit);

    auto pos = ConsumeToken();
    auto error = false;
    Statement* body;

    Result(error, body) = ParseBlock();

    return MakeResult<ClassDeinit>(error, pos, body);
}

/*
 * class_getter
 *     : 'get' identifier block
 *     ;
 * 
 * class_setter
 *     : 'set' identifier block
 *     ;
 */
ParseResult<ClassMember> Parser::ParseClassProperty() {
    assert(m_token == TokenType::Get || m_token == TokenType::Set);

    bool error = false;
    auto type = (m_token == TokenType::Get) ? PropertyType::Getter : PropertyType::Setter;
    auto pos = ConsumeToken();
    StringRef name = ExpectIdentifier(error);
    Statement* body;

    Result(error, body) = ParseBlock();

    return MakeResult<ClassProperty>(error, pos, type, name, body);
}

static OperatorKind ParseOperator(TokenType type) {
    static const OperatorKind Table[] = {
        OperatorKind::Add,
        OperatorKind::InplaceAdd,
        OperatorKind::Sub,
        OperatorKind::InplaceSub,
        OperatorKind::Mul,
        OperatorKind::InplaceMul,
        OperatorKind::Div,
        OperatorKind::InplaceDiv,
        OperatorKind::Mod,
        OperatorKind::InplaceMod,
        OperatorKind::BitAnd,
        OperatorKind::InplaceBitAnd,
        OperatorKind::BitOr,
        OperatorKind::InplaceBitOr,
        OperatorKind::BitNot,
        OperatorKind::BitXor,
        OperatorKind::InplaceBitXor,
        OperatorKind::Less,
        OperatorKind::LessOrEqual,
        OperatorKind::LeftShift,
        OperatorKind::InplaceLeftShift,
        OperatorKind::Grater,
        OperatorKind::GraterOrEqual,
        OperatorKind::RightShift,
        OperatorKind::InplaceRightShift,
        OperatorKind::Equal,
        OperatorKind::NotEqual,
    };

    if (type == BuildScript::TokenType::LeftSquare) {
        // ] token is consumed after return this function.
        return OperatorKind::Index;
    }

    if (type < TokenType::PunctuatorBegin || type > TokenType::NotEqual)
        return OperatorKind::Error;
    else {
        return Table[static_cast<size_t>(TokenType::PunctuatorBegin) - static_cast<size_t>(type)];
    }
}

/*
 * class_operator
 *     : 'operator' op parameters block
 *     ;
 * 
 * op
 *     : '+' | '+=' | '-' | '-=' | '*' | '*=' | '/' | '/=' | '%' | '%='
 *     | '&' | '&=' | '|' | '|=' | '^' | '^='
 *     | '<<' | '<<=' | '>>' | '>>='
 *     | '<' | '<=' | '>' | '>=' | '==' | '!='
 *     | '[' ']'
 *     ;
 */
ParseResult<ClassMember> Parser::ParseClassOperator() {
    assert(m_token == TokenType::Operator);

    auto pos = ConsumeToken();
    auto error = false;
    OperatorKind kind = ParseOperator(m_token.Type);
    Parameters* params;
    Statement* body;

    if (kind == OperatorKind::Error) {
        m_reporter.Report(m_token.Position, ReportID::ParseNotAllowedOperator);
    }
    else
        ConsumeToken(); // Consume operator

    // Extra parsing for [].
    if (kind == OperatorKind::Index) {
        Expect(error, TokenType::RightSquare, ExpectSquare);
    }

    Result(error, params) = ParseParameters();
    Result(error, body) = ParseBlock();
    
    return MakeResult<ClassOperator>(error, pos, kind, params, body);
}

/*
 * class_method
 *     : 'static'? 'def' identifier parameters block
 *     ;
 */
ParseResult<ClassMember> Parser::ParseClassMethod(const SourcePosition& pos, bool isStatic) {
    assert(m_token == TokenType::Def);

    ConsumeToken(); // consume 'def'
    auto error = false;
    StringRef name;
    Parameters* params;
    Statement* body;

    name = ExpectIdentifier(error);
    Result(error, params) = ParseParameters();
    Result(error, body) = ParseBlock();

    return MakeResult<ClassMethod>(error, pos, name, isStatic, params, body);
}