/*
 * Parser.h
 * - Syntax analyzer (a.k.a parser) for BuildScript language.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_PARSER_H
#define BUILDSCRIPT_COMPILER_PARSER_H

#include <string>

#include <BuildScript/Compiler/Lexer.h>
#include <BuildScript/Compiler/ParseResult.h>
#include <BuildScript/Compiler/Token.h>
#include <BuildScript/Utils/NonCopyable.h>
#include <BuildScript/Utils/TypeTraits.h>

namespace BuildScript {
    class ErrorReporter;
    class SourceText;
    class ScriptNode;

    class LabeledStatement;
    class ClassMember;
    class Parameters;

    /**
     * @brief Syntax analyzer (a.k.a parser) for BuildScript language.
     */
    class EXPORT_API Parser : NonCopyable {
    private:
        std::string m_filename;
        Lexer m_lexer;
        ErrorReporter& m_reporter;
        bool m_newline = false;
        Token m_token;

        using ExpectHandler = std::function<void(const SourcePosition&)>;

        inline void GetNextToken() {
            m_newline = false;

            while (true) {
                m_token = m_lexer.LexToken();

                if (m_token == TokenType::EndOfLine || m_token == TokenType::EndOfFile) {
                    m_newline = true;

                    if (m_token == TokenType::EndOfFile)
                        break;
                }
                else if (m_token != TokenType::Comment)
                    break;
            }
        }

        SourcePosition ConsumeToken();

        bool ConsumeIf(TokenType desired, SourcePosition* pos = nullptr);

        void Expect(bool &error, TokenType expected, const ExpectHandler& onfail);

        void ExpectEOL(bool &error, const std::function<void()>& onfail = nullptr);
        
        StringRef ExpectIdentifier(bool &error, SourcePosition* pos = nullptr, const std::function<void()>& onfail = nullptr);

        inline void SkipUntil(TokenType type) {
            while (m_token != type && m_token != TokenType::EndOfFile)
                GetNextToken();
        }

        inline void SkipUntil(TokenType t1, TokenType t2) {
            while (m_token != t1 && m_token != t2 && m_token != TokenType::EndOfFile)
                GetNextToken();
        }

        void ConsumeInvalidStatements();

        const std::function<void()> SkipToEOL = [&]() {
            while (!m_newline)
                GetNextToken();
        };

        const ExpectHandler ExpectColon
            = [&](const SourcePosition& pos) {
                m_reporter.Report(pos, ReportID::ParseExpectColon);
            };

        const ExpectHandler ExpectParen
            = [&](const SourcePosition& pos) {
                m_reporter.Report(pos, ReportID::ParseParenNotClosed);
            };

        const ExpectHandler ExpectSquare
            = [&](const SourcePosition& pos) {
                m_reporter.Report(pos, ReportID::ParseSquareNotClosed);
            };

        // Parser.Declaration.cpp
        DeclResult ParseDeclaration();
        DeclResult ParseExportDeclaration();
        DeclResult ParseImportDeclaration();
        DeclResult ParseTaskDeclaration();
        DeclResult ParseClassDeclaration();
        DeclResult ParseFunctionDeclaration();
        ParseResult<Parameters> ParseParameters();

        // Parser.ClassMember.cpp
        ParseResult<ClassMember> ParseClassStatic();
        ParseResult<ClassMember> ParseClassInit();
        ParseResult<ClassMember> ParseClassDeinit();
        ParseResult<ClassMember> ParseClassProperty();
        ParseResult<ClassMember> ParseClassOperator();
        ParseResult<ClassMember> ParseClassMethod(const SourcePosition&, bool);

        // Parser.Statement.cpp
        StmtResult ParseStatement();
        StmtResult ParseBlock();
        StmtResult ParseIfStatement();
        ParseResult<LabeledStatement> ParseLabeledStatement();
        StmtResult ParseMatchStatement();
        StmtResult ParseForStatement();
        StmtResult ParseWhileStatement();
        StmtResult ParseTryStatement();
        StmtResult ParseRaiseStatement();
        StmtResult ParseBreakStatement();
        StmtResult ParseContinueStatement();
        StmtResult ParseReturnStatement();
        StmtResult ParseEmptyStatement();
        StmtResult ParseAssertStatement();
        StmtResult ParseVariableDeclaration();

        // Parser.Expression.cpp
        ExprResult ParseExpression() { return ParseTernaryExpression(); }
        ExprResult ParseBooleanExpression() { return ParseBinaryExpression(); }
        ExprResult ParseTernaryExpression();
        ExprResult ParseBinaryExpression();
        ExprResult ParseUnaryExpression();
        ExprResult ParseCastExpression();
        ExprResult ParsePostfixExpression();
        ExprResult ParsePrimaryExpression();
        ExprResult ParseParenthesis();
        ExprResult ParseString();
        ExprResult ParseMap();
        ExprResult ParseList();
        ExprResult ParseClosure(bool, const SourcePosition&, std::vector<Expression*>&);
        ExprResult ParseNamedTuple(const SourcePosition&, ExprResult);

    public:
        /**
         * @brief Construct Parser.
         * @param source the source to parse.
         * @param reporter the reporter.
         * @see BuildScript::SourceText
         * @see BuildScript::ErrorReporter
         */
        Parser(SourceText& source, ErrorReporter& reporter);

        /**
         * @brief Parse script.
         * @return Parsed script.
         * @see BuildScript::ScriptNode
         */
        ScriptNode* ParseScript();
    }; // end class Parser
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_PARSER_H