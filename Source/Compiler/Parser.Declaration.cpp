/*
 * Parser.Declaration.cpp
 * - Syntax analyzer (a.k.a parser) for BuildScript language.
 *   This file contains part of Parser that parse declarations.
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
DeclResult MakeResult(bool status, Args&&... args) {
    T* decl = new T(std::forward<Args>(args)...);
    return DeclResult(status, decl);
}

DeclResult Parser::ParseDeclaration() {
    switch (m_token.Type) {
        case TokenType::Export:
            return ParseExportDeclaration();

        case TokenType::Import:
            return ParseImportDeclaration();

        case TokenType::Task:
            return ParseTaskDeclaration();

        case TokenType::Class:
            return ParseClassDeclaration();

        case TokenType::Def:
            return ParseFunctionDeclaration();

        default:
            return nullptr;
    }
}

/*
 * export_declaration
 *     : 'export' identifier ('=' expression)? EOL
 *     | 'export' '{'  '}'
 *     ;
 */
DeclResult Parser::ParseExportDeclaration() {
    assert(m_token == TokenType::Export);

    auto pos = ConsumeToken();
    auto error = false;
    std::vector<Property> exports;
    SourcePosition namePos;
    StringRef name;
    Expression* value = nullptr;

    if (ConsumeIf(TokenType::LeftBrace)) {
        do {
            if (m_token == TokenType::EndOfFile) {
                m_reporter.Report(m_token.Position, ReportID::ParseUnexpectedEOF);
                break;
            }

            name = ExpectIdentifier(error, &namePos);

            Expect(error, TokenType::Assign, [&](const SourcePosition& pos) {
                m_reporter.Report(pos, ReportID::ParseExpectAssign);
            });

            Result(error, value) = ParseExpression();

            ExpectEOL(error, SkipToEOL);

            exports.emplace_back(namePos, name, value);
        } while (!ConsumeIf(TokenType::RightBrace));
    }
    else {
        name = ExpectIdentifier(error);

        if (ConsumeIf(TokenType::Assign))
            Result(error, value) = ParseExpression();
        
        exports.emplace_back(namePos, name, value);
    }

    ExpectEOL(error);

    return MakeResult<ExportDeclaration>(error, pos, std::move(exports));
}

/*
 * import_declaration
 *     : 'import' string_literal EOL
 *     | 'import' '[' string_list ']' EOL
 *     ;
 */
DeclResult Parser::ParseImportDeclaration() {
    assert(m_token == TokenType::Import);

    auto pos = ConsumeToken();
    auto error = false;
    std::vector<Expression*> imports;

    auto expectString = [&]() {
        if (m_token != TokenType::String) {
            m_reporter.Report(m_token.Position, ReportID::ParseExpectString);
            error = true;
            return true;
        } else {
            Expression* str;
            Result(error, str) = ParseString();

            imports.push_back(str);
            return false;
        }
    };

    if (ConsumeIf(TokenType::LeftSquare)) {
        do {
            if (expectString()) {
                SkipUntil(TokenType::Comma, TokenType::RightSquare);
            }
        } while (ConsumeIf(TokenType::Comma));

        Expect(error, TokenType::RightSquare, ExpectSquare);
    }
    else {
        if (expectString()) {
            SkipToEOL();
        }
    }

    ExpectEOL(error);

    return MakeResult<ImportDeclaration>(error, pos, std::move(imports));
}

/*
 * task_declaration
 *     : 'task' task_name extends_clause? dependson_clause? task_body
 *     ;
 * 
 * task_name
 *     : identifier
 *     ;
 * 
 * extends_clause
 *     : 'extends' identifier
 *     ;
 * 
 * dependson_clause
 *     : 'dependsOn' identifier_list
 *     ;
 * 
 * task_body
 *     : '{'  '}'
 *     ;
 */
DeclResult Parser::ParseTaskDeclaration() {
    assert(m_token == TokenType::Task);

    auto pos = ConsumeToken();
    auto error = false;
    StringRef name, extends;
    std::vector<StringRef> dependencies;
    InputsClause inputs;
    OutputsClause outputs;
    std::vector<Property> properties;
    Statement* doBody = nullptr;
    Statement* doFirstBody = nullptr;
    Statement* doLastBody = nullptr;
    
    name = ExpectIdentifier(error);

    if (ConsumeIf(TokenType::Extends))
        extends = ExpectIdentifier(error);

    if (ConsumeIf(TokenType::DependsOn)) {
        do {
            StringRef deps = ExpectIdentifier(error);

            if (deps)
                dependencies.emplace_back(deps);
        } while (ConsumeIf(TokenType::Comma));
    }

    Expect(error, TokenType::LeftBrace, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseExpectBrace);
        SkipUntil(TokenType::LeftBrace, TokenType::RightBrace);
        ConsumeIf(TokenType::LeftBrace);
    });

    auto loop = true;
    while (loop) {
        switch (m_token.Type) {
            case TokenType::Inputs: {
                if (inputs) {
                    m_reporter.Report(m_token.Position, ReportID::ParseOverridesInputs);
                }

                inputs.Position = ConsumeToken();

                Result(error, inputs.Value) = ParseExpression();

                if (ConsumeIf(TokenType::With))
                    Result(error, inputs.With) = ParseExpression();
                
                ExpectEOL(error, SkipToEOL);
                break;
            }
            
            case TokenType::Outputs: {
                if (outputs) {
                    m_reporter.Report(m_token.Position, ReportID::ParseOverridesOutputs);
                }

                outputs.Position = ConsumeToken();
                Result(error, outputs.Value) = ParseExpression();

                if (ConsumeIf(TokenType::From))
                    Result(error, outputs.From) = ParseExpression();
                
                ExpectEOL(error, SkipToEOL);
                break;
            }
            

            case TokenType::Do: {
                if (doBody != nullptr) {
                    m_reporter.Report(m_token.Position, ReportID::ParseOverridesDo);
                    delete doBody;
                }

                ConsumeToken();
                Result(error, doBody) = ParseBlock();
                break;
            }

            case TokenType::DoFirst: {
                if (doFirstBody != nullptr) {
                    m_reporter.Report(m_token.Position, ReportID::ParseOverridesDoFirst);
                    delete doFirstBody;
                }

                ConsumeToken();
                Result(error, doFirstBody) = ParseBlock();
                break;
            }
            
            case TokenType::DoLast: {
                if (doLastBody != nullptr) {
                    m_reporter.Report(m_token.Position, ReportID::ParseOverridesDoLast);
                    delete doLastBody;
                }

                ConsumeToken();
                Result(error, doLastBody) = ParseBlock();
                break;
            }
            
            case TokenType::Identifier: {
                StringRef name = m_token.Image;
                Expression* value;
                auto pos = ConsumeToken();

                Expect(error, TokenType::Assign, [&](const SourcePosition& pos) {
                    m_reporter.Report(pos, ReportID::ParseExpectAssign);
                });

                Result(error, value) = ParseExpression();

                ExpectEOL(error, SkipToEOL);
                
                properties.emplace_back(pos, name, value);
                break;
            }
            
            case TokenType::RightBrace:
                ConsumeToken();
                loop = false;
                break;
            
            case TokenType::EndOfFile:
                m_reporter.Report(m_token.Position, ReportID::ParseUnexpectedEOF);
                loop = false;
                break;
            
            default:
                ConsumeInvalidStatements();
                error = true;
                break;
        }
    }

    return MakeResult<TaskDeclaration>(error, pos, name, extends, std::move(dependencies), std::move(inputs),
                                       std::move(outputs), std::move(properties), doBody, doFirstBody, doLastBody);
}

/*
 * parameters
 *     : '(' ')'
 *     ;
 */
ParseResult<Parameters> Parser::ParseParameters() {
    std::vector<Parameters::Parameter> params;
    Parameters::Parameter varadic;
    auto error = false;

    Expect(error, TokenType::LeftParen, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseExpectParen);
        SkipUntil(TokenType::LeftParen, TokenType::RightParen);
    });

    if (m_token != TokenType::RightParen) {
        do {
            SourcePosition pos;
            StringRef param = ExpectIdentifier(error, &pos);

            if (param)
                params.emplace_back(param, pos);
        } while (ConsumeIf(TokenType::Comma));

        if (ConsumeIf(TokenType::Ellipsis)) {
            varadic = params.back();
            params.pop_back();
        }
    }

    Expect(error, TokenType::RightParen, ExpectParen);

    return ParseResult<Parameters>(error, new Parameters(std::move(params), varadic));
}

/*
 * class_declaration
 *     : 'class' class_name extends_clause? class_body
 *     ;
 * 
 * class_body
 *     : '{' class_member* '}'
 *     ;
 * 
 * class_member
 *     : class_static_member
 *     | class_initializer
 *     | class_deinitializer
 *     | class_method
 *     | class_getter
 *     | class_setter
 *     | class_operator
 *     ;
 *
 * class_static_member
 *     : 'static' identifier '=' expression EOL
 *     ;
 * 
 * class_initializer
 *     : 'init' parameters block
 *     ;
 * 
 * class_deinitializer
 *     : 'deinit' block
 *     ;
 * 
 * class_method
 *     : 'def' identifier parameters block
 *     ;
 * 
 * class_getter
 *     : 'get' identifier block
 *     ;
 * 
 * class_setter
 *     : 'set' identifier block
 *     ;
 * 
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
DeclResult Parser::ParseClassDeclaration() {
    assert(m_token == TokenType::Class);

    auto pos = ConsumeToken();
    auto error = false;
    StringRef name;
    StringRef extends;
    std::vector<ClassMember*> members;
    
    name = ExpectIdentifier(error);

    if (ConsumeIf(TokenType::Extends))
        extends = ExpectIdentifier(error);

    Expect(error, TokenType::LeftBrace, [&](const SourcePosition& pos) {
        m_reporter.Report(pos, ReportID::ParseExpectBrace);
        SkipUntil(TokenType::LeftBrace, TokenType::RightBrace);
    });

    auto loop = true;
    while (loop) {
        ParseResult<ClassMember> member = nullptr;

        switch (m_token.Type) {
            case TokenType::Static:
                member = ParseClassStatic();
                break;

            case TokenType::Init:
                member = ParseClassInit();
                break;

            case TokenType::Deinit:
                member = ParseClassDeinit();
                break;

            case TokenType::Def:
                member = ParseClassMethod(m_token.Position, false);
                break;

            case TokenType::Operator:
                member = ParseClassOperator();
                break;

            case TokenType::Get:
            case TokenType::Set:
                member = ParseClassProperty();
                break;

            case TokenType::RightBrace:
                ConsumeToken();
                loop = false;
                break;
            
            case TokenType::EndOfFile:
                m_reporter.Report(m_token.Position, ReportID::ParseUnexpectedEOF);
                error = true;
                loop = false;
                break;

            default:
                ConsumeInvalidStatements();
                error = true;
                break;
        }

        if (member.HasValue()) {
            members.emplace_back(member.GetValue());
            error |= member.HasError();
        }
    }

    return MakeResult<ClassDeclaration>(error, pos, name, extends, std::move(members));
}

/*
 * function_declaration
 *     : 'def' function_name parameters body
 *     ;
 */
DeclResult Parser::ParseFunctionDeclaration() {
    assert(m_token == TokenType::Def);

    auto pos = ConsumeToken();
    auto error = false;
    StringRef name;
    Parameters* params;
    Statement* body;

    name = ExpectIdentifier(error);
    Result(error, params) = ParseParameters();
    Result(error, body) = ParseBlock();

    return MakeResult<FunctionDeclaration>(error, pos, name, params, body);
}