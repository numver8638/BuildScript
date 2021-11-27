/*
 * Parser.h
 * - Syntax analyzer for BuildScript language.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_PARSE_PARSER_H
#define BUILDSCRIPT_COMPILER_PARSE_PARSER_H

#include <BuildScript/Assert.h>
#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/Parse/ParserBase.h>
#include <BuildScript/Utils/Functions.h>

namespace BuildScript {
    class Context; // Defined in <BuildScript/Compiler/Context.h>
    class Parameters;

    /**
     * @brief Syntax analyzer for BuildScript language.
     */
    class Parser final : protected ParserBase {
    private:
        Context& m_context;

        bool CheckForClosure();

        /**
         * @brief .
         */
        enum SkipFlag {
            StopBeforeBrace,
            StopAfterBrace
        }; // end enum SkipFlag

        SourceRange SkipBraces(SkipFlag, const Predicate<TokenType>& = [](auto) { return false; });
        SourceRange SkipToEOL();
        SourceRange SkipTokenExpr();

        // Parser.Misc.cpp
        Parameters* ParseParameters();
        void ParseNameList(std::vector<Identifier>&, std::vector<SourcePosition>&);
        void ParseExpressionList(std::vector<Expression*>&, std::vector<SourcePosition>&);

        // Parser.Declaration.cpp
        Declaration* ParseDeclaration();
        Declaration* ParseImportDeclaration();
        Declaration* ParseExportDeclaration();
        Declaration* ParseFunctionDeclaration();
        Declaration* ParseClassDeclaration();
        Declaration* ParseClassMember();
        Declaration* ParseClassInit();
        Declaration* ParseClassDeinit();
        Declaration* ParseClassField(SourcePosition, SourcePosition);
        Declaration* ParseClassMethod(SourcePosition);
        Declaration* ParseClassProperty();
        Declaration* ParseClassOperator();
        Declaration* ParseTaskDeclaration();
        Declaration* ParseVariableDeclaration();

        // Parser.Statement.cpp
        ASTNode* ParseLocalDeclarationOrStatement();
        ASTNode* ParseStatement();
        Statement* ParseBody();
        Statement* ParseIfStatement();
        Statement* ParseMatchStatement();
        Statement* ParseForStatement();
        Statement* ParseWhileStatement();
        Statement* ParseWithStatement();
        Statement* ParseTryStatement();
        Statement* ParseBreakStatement();
        Statement* ParseContinueStatement();
        Statement* ParseReturnStatement();
        Statement* ParseAssertStatement();
        Statement* ParsePassStatement();

        // Parser.Expression.cpp
        Expression* ParseExpression() { return ParseTernaryExpression(); }
        Expression* ParseBooleanExpression() { return ParseBinaryExpression(); }
        Expression* ParseConstantExpression() { return ParseBinaryExpression(); }
        Expression* ParseTernaryExpression();
        Expression* ParseBinaryExpression(int rank = 0);
        Expression* ParseUnaryExpression();
        Expression* ParseDefinedExpression();
        Expression* ParseRaiseExpression();
        Expression* ParsePostfixExpression();
        Expression* ParsePrimaryExpression();
        Expression* ParseParenthesis();
        Expression* ParseString();
        Expression* ParseList();
        Expression* ParseMap();
        Expression* ParseClosure();

    public:
        /**
         * @brief Syntax analyzer for BuildScript language.
         * @param source a source text to parse.
         * @param reporter an @c ErrorReporter to collect errors.
         * @param context a @c Context.
         */
        Parser(SourceText& source, ErrorReporter& reporter, Context& context)
            : ParserBase(source, reporter), m_context(context) {}

        /**
         * @brief Parse script.
         * @return an @c ASTNode representing a script.
         */
        ASTNode* Parse();
    }; // end class Parser
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_PARSE_PARSER_H