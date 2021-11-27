/*
 * Parser.Declaration.cpp
 * - Syntax analyzer for BuildScript language.
 *   This file contains partial code of parser that parses declaration.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Parse/Parser.h>

#include <BuildScript/Compiler/AST/Declarations.h>
#include <BuildScript/Compiler/AST/Expressions.h>
#include <BuildScript/Compiler/ErrorReporter.h>
#include <BuildScript/Compiler/ErrorReporter.ReportID.h>

using namespace BuildScript;

static inline OperatorKind ToOperatorKind(const Token& token) {
    if ((TokenType::Add <= token.Type) && (token.Type <= TokenType::InplaceBitXor)) {
        static const OperatorKind table[] = {
            OperatorKind::Add,
            OperatorKind::Sub,
            OperatorKind::Mul,
            OperatorKind::Div,
            OperatorKind::Mod,
            OperatorKind::LeftShift,
            OperatorKind::RightShift,
            OperatorKind::BitAnd,
            OperatorKind::BitOr,
            OperatorKind::BitNot,
            OperatorKind::BitXor,
            OperatorKind::Invalid,  // <
            OperatorKind::Invalid,  // <=
            OperatorKind::Invalid,  // >
            OperatorKind::Invalid,  // >=
            OperatorKind::Invalid,  // ==
            OperatorKind::Invalid,  // !=
            OperatorKind::Invalid,  // =
            OperatorKind::InplaceAdd,
            OperatorKind::InplaceSub,
            OperatorKind::InplaceMul,
            OperatorKind::InplaceDiv,
            OperatorKind::InplaceMod,
            OperatorKind::InplaceLeftShift,
            OperatorKind::InplaceRightShift,
            OperatorKind::InplaceBitAnd,
            OperatorKind::InplaceBitOr,
            OperatorKind::InplaceBitXor,
        };

        return table[static_cast<size_t>(token.Type) - static_cast<size_t>(TokenType::Add)];
    }
    else if (token == TokenType::LeftSquare) {
        return OperatorKind::Index;
    }
    else {
        return OperatorKind::Invalid;
    }
}

Declaration* Parser::ParseDeclaration() {
    switch (m_token.Type) {
        case TokenType::Import:
            return ParseImportDeclaration();

        case TokenType::Export:
            return ParseExportDeclaration();

        case TokenType::Def:
            return ParseFunctionDeclaration();

        case TokenType::Class:
            return ParseClassDeclaration();

        case TokenType::Task:
            return ParseTaskDeclaration();

        case TokenType::Const:
        case TokenType::Var:
        case TokenType::Static:
            // 'static' is not a valid variable declaration but accepted for precise error reporting.
            return ParseVariableDeclaration();

        default:
            return nullptr;
    }
}

/*
 * import_declaration
 *  : 'import' string EOL
 *  ;
 */
Declaration* Parser::ParseImportDeclaration() {
    assert(m_token == TokenType::Import);

    auto _import = ConsumeToken();
    Expression* name;

    if (m_token == TokenType::String) {
        name = ParseString();
    }
    else {
        m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectString);
        name = InvalidExpression::Create(m_context, SkipToEOL());
    }

    RequireEOL();

    return ImportDeclaration::Create(m_context, _import, name);
}

/*
 * export_declaration
 *  : 'export' identifier ('=' expression)? EOL
 *  ;
 */
Declaration* Parser::ParseExportDeclaration() {
    assert(m_token == TokenType::Export);

    auto _export = ConsumeToken();
    auto name = RequireIdentifier();
    SourcePosition assign;
    Expression* value = nullptr;

    // Export declaration can omit assignment.
    if (ConsumeIf(TokenType::Assign, assign)) {
        value = ParseExpression();
    }

    RequireEOL();

    return ExportDeclaration::Create(m_context, _export, std::move(name), assign, value);
}

/*
 * function_declaration
 *  : 'def' identifier parameter body
 *  ;
 */
Declaration* Parser::ParseFunctionDeclaration() {
    assert(m_token == TokenType::Def);

    auto def = ConsumeToken();
    auto name = RequireIdentifier();
    auto* params = ParseParameters();
    auto* body = ParseBody();

    return FunctionDeclaration::Create(m_context, def, std::move(name), params, body);
}

/*
 * class_declaration
 *  : 'class' identifier extends_clause? '{' class_member* '}'
 *  ;
 *
 * class_member
 *  : class_init
 *  | class_deinit
 *  | class_field
 *  | class_method
 *  | class_property
 *  | class_operator
 *  ;
 */
Declaration* Parser::ParseClassDeclaration() {
    assert(m_token == TokenType::Class);

    const auto ParseExtends = [&](SourcePosition &extends, Identifier &name) -> void {
        if (ConsumeIf(TokenType::Extends, extends)) {
            name = RequireIdentifier();
        }
    };

    auto _class = ConsumeToken();
    auto name = RequireIdentifier();
    SourcePosition extends;
    Identifier extendsName;

    ParseExtends(extends, extendsName);

    auto open = RequireToken(TokenType::LeftBrace);

    std::vector<Declaration*> nodes;
    while (!OneOf<TokenType::RightBrace, TokenType::EndOfFile>()) {
        nodes.push_back(ParseClassMember());
    }

    auto close = RequireToken(TokenType::RightBrace);

    return ClassDeclaration::Create(
        m_context, _class, std::move(name), extends, std::move(extendsName), open, nodes, close);
}

Declaration* Parser::ParseClassMember() {
    SourceRange _const, _static;

    while (OneOf<TokenType::Const, TokenType::Static, TokenType::Var>()) {
        // Handle common typo: use 'var' in class field declaration. Fields declared in class are static.
        if (m_token == TokenType::Var) {
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseVarIsNotAllowedInClass)
                      .Remove(m_token.Range);
        }
        else if (_const || _static) {
            m_reporter.Report(m_token.GetPosition(),ReportID::ParseRedundantKeyword, m_token.TypeToString())
                      .Remove(m_token.Range);
        }
        else {
            m_token == TokenType::Const ? (_const = m_token.Range) : (_static = m_token.Range);
        }

        ConsumeToken();
    }

    const auto CheckSpecifier = [&](const char* name) {
        if (_const) {
            m_reporter.Report(_const.Begin, ReportID::ParseSpecifierNotAllowed, name, "const")
                      .Remove(_const);
        }
        else if (_static) {
            m_reporter.Report(_static.Begin, ReportID::ParseSpecifierNotAllowed, name, "static")
                      .Remove(_static);
        }
    };
    const auto SkipFilter = [](TokenType type) {
        // Stop before valid start point of class member.
        switch (type) {
            case TokenType::Identifier:
            case TokenType::Init:
            case TokenType::Deinit:
            case TokenType::Def:
            case TokenType::Get:
            case TokenType::Set:
            case TokenType::Operator:
            case TokenType::Const:
            case TokenType::Static:
                return true;

            default:
                return false;
        }
    };

    switch (m_token.Type) {
        case TokenType::Init:
            CheckSpecifier("initializer"); // Any specifiers are invalid.
            return ParseClassInit();

        case TokenType::Deinit:
            CheckSpecifier("deinitializer"); // Any specifiers are invalid.
            return ParseClassDeinit();

        case TokenType::Get:
        case TokenType::Set:
            CheckSpecifier("property"); // Any specifiers are invalid.
            return ParseClassProperty();

        case TokenType::Def:
            if (_const) {
                m_reporter.Report(_const.Begin, ReportID::ParseSpecifierNotAllowed, "method", "const")
                          .Remove(_const);
            }
            return ParseClassMethod(_static.Begin);

        case TokenType::Identifier:
            return ParseClassField(_const.Begin, _static.Begin);

        case TokenType::Operator:
            CheckSpecifier("operator"); // Any specifiers are invalid.
            return ParseClassOperator();

        default:
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectClassMember);
            return InvalidDeclaration::Create(m_context, SkipBraces(StopBeforeBrace, SkipFilter));
    }
}

/*
 * class_init
 *  : 'init' parameters block
 *  ;
 */
Declaration* Parser::ParseClassInit() {
    assert(m_token == TokenType::Init);

    auto init = ConsumeToken();
    auto* params = ParseParameters();
    auto* body = ParseBody();

    return ClassInitDeclaration::Create(m_context, init, params, body);
}

/*
 * class_deinit
 *  : 'deinit' block
 *  ;
 */
Declaration* Parser::ParseClassDeinit() {
    assert(m_token == TokenType::Deinit);

    auto deinit = ConsumeToken();
    auto* body = ParseBody();

    return ClassDeinitDeclaration::Create(m_context, deinit, body);
}

/*
 * class_field
 *  : storage_specifier identifier '=' expression EOL
 *  ;
 *
 * storage_specifier
 *  : 'const'
 *  | 'static'
 *  ;
 */
Declaration* Parser::ParseClassField(SourcePosition _const, SourcePosition _static) {
    assert(m_token == TokenType::Identifier);

    auto name = RequireIdentifier(); // always succeed.
    auto assign = RequireToken(TokenType::Assign);
    auto* value = ParseExpression();

    RequireEOL();

    return ClassFieldDeclaration::Create(m_context, _const, _static, std::move(name), assign, value);
}

/*
 * class_method
 *  : ('static')? 'def' identifier parameters block
 *  ;
 */
Declaration* Parser::ParseClassMethod(SourcePosition _static) {
    assert(m_token == TokenType::Def);

    auto def = ConsumeToken();

    // Check reversed 'static' keyword.
    if (ConsumeIf(TokenType::Static, _static)) {
        m_reporter.Report(_static, ReportID::ParseReversedStaticKeyword);
    }

    auto name = RequireIdentifier();
    auto* params = ParseParameters();
    auto* body = ParseBody();

    return ClassMethodDeclaration::Create(m_context, _static, def, name, params, body);
}

/*
 * class_property
 *  : 'get' identifier block
 *  | 'set' identifier block
 *  ;
 */
Declaration* Parser::ParseClassProperty() {
    assert(m_token == TokenType::Get || m_token == TokenType::Set);

    SourcePosition get, set;
    (m_token == TokenType::Get) ? (get = ConsumeToken()) : (set = ConsumeToken());
    auto name = RequireIdentifier();
    auto* body = ParseBody();

    return ClassPropertyDeclaration::Create(m_context, get, set, std::move(name), body);
}

/*
 * class_operator
 *  : 'operator' operator parameters block
 *  ;
 *
 * operator
 *  : '+' | '-' | '*' | '/' | '%'
 *  | '&' | '|' | '^' | '~' |
 *  | '<<' | '>>'
 *  | '+=' | '-=' | '*=' | '/=' | '%='
 *  | '&=' | '|=' | '^='
 *  | '<<=' | '>>='
 *  | '[' ']'
 *  ;
 */
Declaration* Parser::ParseClassOperator() {
    assert(m_token == TokenType::Operator);

    auto _operator = ConsumeToken();
    auto kind = ToOperatorKind(m_token);
    std::array<SourcePosition, 2> pos;

    if (kind == OperatorKind::Invalid) {
        // Report error
        auto pos = m_token.GetPosition();
        auto type = m_token.Type;

        m_reporter.Report(pos, ReportID::ParseNotAOperator, Token::TypeToString(type));

        if ((TokenType::Less <= type) && (type <= TokenType::GraterOrEqual)) {
            ConsumeToken();
            m_reporter.Report(pos, ReportID::ParseOverrideCompare, Token::TypeToString(type));
        }
        else if (type == TokenType::Equal || type == TokenType::NotEqual) {
            ConsumeToken();
            m_reporter.Report(pos, ReportID::ParseOverrideEquals, Token::TypeToString(type));
        }
    }
    else {
        pos[0] = ConsumeToken();

        if (kind == OperatorKind::Index) {
            pos[1] = RequireToken(TokenType::RightSquare);
        }
    }

    auto* params = ParseParameters();
    auto* body = ParseBody();

    return ClassOperatorDeclaration::Create(m_context, _operator, kind, pos, params, body);
}

/*
 * task_declaration
 *  : 'task' identifier extends_clause? dependsOn_clause? task_body
 *  ;
 *
 * extends_clause
 *  : 'extends' identifier
 *  ;
 *
 * dependsOn_clause
 *  : 'dependsOn' names
 *  ;
 *
 * names
 *  : identifier
 *  | names ',' identifier
 *  ;
 *
 * task_body
 *  : '{' task_item* '}'
 *  ;
 *
 * task_item
 *  : inputs_clause
 *  | outputs_clause
 *  | property
 *  | do_clause
 *  | doFirst_clause
 *  | doLast_clause
 *  ;
 *
 * inputs_clause
 *  : 'inputs' expression ('with' expression)? EOL
 *  ;
 *
 * outputs_clause
 *  : 'outputs' expression ('from' expression)? EOL
 *  ;
 *
 * property
 *  : identifier '=' expression EOL
 *  ;
 *
 * do_clause
 *  : 'do' block
 *  ;
 *
 * doFirst_clause
 *  : 'doFirst' block
 *  ;
 *
 * doLast_clause
 *  : 'doLast' block
 *  ;
 */
Declaration* Parser::ParseTaskDeclaration() {
    assert(m_token == TokenType::Task);

    const auto ParseExtends = [&](SourcePosition &extends, Identifier &name) -> void {
        if (ConsumeIf(TokenType::Extends, extends)) {
            name = RequireIdentifier();
        }
    };
    const auto ParseDependsOn =
        [&](SourcePosition &dependsOn, std::vector<Identifier> &names, std::vector<SourcePosition> &commas)  {
        if (ConsumeIf(TokenType::DependsOn, dependsOn)) {
            ParseNameList(names, commas);
        }
    };
    const auto SkipFilter = [](TokenType type) {
        switch (type) {
            case TokenType::Inputs:
            case TokenType::Outputs:
            case TokenType::Do:
            case TokenType::DoFirst:
            case TokenType::DoLast:
            case TokenType::Identifier:
                return true;

            default:
                return false;
        }
    };

    auto task = ConsumeToken();
    auto name = RequireIdentifier();
    SourcePosition extends, dependsOn;
    Identifier extendsName;
    std::vector<Identifier> depnames;
    std::vector<SourcePosition> commas;

    ParseExtends(extends, extendsName);
    ParseDependsOn(dependsOn, depnames, commas);

    // Check for reversed extends/dependsOn clause
    if (extends && m_token == TokenType::Extends) {
        m_reporter.Report(m_token.GetPosition(), ReportID::ParseReversedExtendsClause);

        ParseExtends(extends, extendsName);
    }

    auto open = RequireToken(TokenType::LeftBrace);

    std::vector<Declaration*> nodes;
    while (!OneOf<TokenType::RightBrace, TokenType::EndOfFile>()) {
        Declaration* node;

        switch (m_token.Type) {
            // Inputs clause
            case TokenType::Inputs: {
                auto pos = ConsumeToken();
                auto* value = ParseExpression();
                SourcePosition with;
                Expression* withValue = nullptr;

                if (ConsumeIf(TokenType::With, with)) {
                    withValue = ParseExpression();
                }

                RequireEOL();
                node = TaskInputsDeclaration::Create(m_context, pos, value, with, withValue);
                break;
            }

            // Outputs clause
            case TokenType::Outputs: {
                auto pos = ConsumeToken();
                auto* value = ParseExpression();
                SourcePosition from;
                Expression* fromValue = nullptr;

                if (ConsumeIf(TokenType::From, from)) {
                    fromValue = ParseExpression();
                }

                RequireEOL();
                node = TaskOutputsDeclaration::Create(m_context, pos, value, from, fromValue);
                break;
            }

            // Property
            case TokenType::Identifier: {
                auto propName = RequireIdentifier(); // always succeed.
                auto assign = RequireToken(TokenType::Assign);
                auto* value = ParseExpression();

                RequireEOL();

                node = TaskPropertyDeclaration::Create(m_context, std::move(propName), assign, value);
                break;
            }

            // Action clauses
            case TokenType::Do:
            case TokenType::DoFirst:
            case TokenType::DoLast: {
                ActionKind kind;

                switch (m_token.Type) {
                    case TokenType::Do:
                        kind = ActionKind::Do;
                        break;

                    case TokenType::DoFirst:
                        kind = ActionKind::DoFirst;
                        break;

                    case TokenType::DoLast:
                        kind = ActionKind::DoLast;
                        break;

                    default:
                        NOT_REACHABLE;
                        break;
                }

                auto pos = ConsumeToken();
                auto* body = ParseBody();

                node = TaskActionDeclaration::Create(m_context, kind, pos, body);
                break;
            }

            default:
                m_reporter.Report(m_token.GetPosition(), ReportID::ParseExpectTaskItem);
                node = InvalidDeclaration::Create(m_context, SkipBraces(StopBeforeBrace, SkipFilter));
                break;
        }

        nodes.push_back(node);
    }

    auto close = RequireToken(TokenType::RightBrace);

    return TaskDeclaration::Create(m_context, task, std::move(name), extends, std::move(extendsName),
        dependsOn, depnames, commas, open, nodes, close);
}

/*
 * variable_declaration
 *  : ('var'|'const') identifier '=' expression EOL
 *  ;
 */
Declaration* Parser::ParseVariableDeclaration() {
    SourcePosition _const, var;

    while (OneOf<TokenType::Const, TokenType::Static, TokenType::Var>()) {
        // Handle common typo: use 'static' in variable declaration.
        if (m_token == TokenType::Static) {
            m_reporter.Report(m_token.GetPosition(), ReportID::ParseVariableCannotBeStatic)
                      .Remove(m_token.Range);
        }
        else if (_const || var) {
            m_reporter.Report(m_token.GetPosition(),ReportID::ParseRedundantKeyword, m_token.TypeToString())
                      .Remove(m_token.Range);
        }
        else {
            (m_token == TokenType::Const) ? (_const = m_token.GetPosition()) : (var = m_token.GetPosition());
        }

        ConsumeToken();
    }

    if (!_const && !var) {
        // Insert phony position
        var = m_token.GetPosition();
    }

    auto name = RequireIdentifier();
    auto assign = RequireToken(TokenType::Assign);
    auto* value = ParseExpression();

    RequireEOL();

    return VariableDeclaration::Create(m_context, _const, var, std::move(name), assign, value);
}