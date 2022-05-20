/*
 * SemanticAnalyzer.h
 * - Verify and analyze AST given by Parser.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_H
#define BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_H

#include <stack>
#include <string_view>

#include <BuildScript/Compiler/AST/ASTWalker.h>
#include <BuildScript/Compiler/Context.h>
#include <BuildScript/Utils/NonCopyable.h>
#include <BuildScript/Assert.h>

namespace BuildScript {
    class ErrorReporter; // Defined in <BuildScript/Compiler/ErrorReporter.h>
    class LocalScope;
    class Symbol;
    class Identifier;

    /**
     * @brief Verify and analyze AST given by Parser.
     */
    class SemanticAnalyzer final : ASTWalker, NonCopyable {
        using super = ASTWalker;

        // RAII objects for convenience
        template <typename>
        friend struct AutoScope;

    private:
        ErrorReporter& m_reporter;
        Context& m_context;

        LocalScope* m_global = nullptr;
        LocalScope* m_scope = nullptr;

        Symbol* UnusedSymbol = nullptr;

        LocalScope& GetGlobal() {
            NEVER_BE_NULL(m_global);
            return *m_global;
        }

        LocalScope& GetCurrentScope() {
            NEVER_BE_NULL(m_scope);
            return *m_scope;
        }

        template <typename SymbolType, typename... ArgsType>
        SymbolType* CreateLocalSymbol(ArgsType&&... args);

        template <typename SymbolType, typename... ArgsType>
        SymbolType* CreateGlobalSymbol(ArgsType&&... args);

        Symbol* FindTypeSymbol(const Identifier&);

        void CheckRedefinition(const Identifier&, std::string_view);
        void CheckRedefinition(const Identifier&, int, bool, std::string_view);

        Symbol* BuildClassSymbol(ClassDeclaration*);
        Symbol* BuildTaskSymbol(TaskDeclaration*);

        // Nodes to be checked semantically.
#define NODES_TO_CHECK(V) \
        V(ParameterList, _) \
        V(ScriptDeclaration, _) \
        V(ImportDeclaration, _) \
        V(ExportDeclaration, _) \
        V(FunctionDeclaration, _) \
        V(ClassDeclaration, _) \
        V(ClassInitDeclaration, _) \
        V(ClassDeinitDeclaration, _) \
        V(ClassFieldDeclaration, _) \
        V(ClassMethodDeclaration, _) \
        V(ClassPropertyDeclaration, _) \
        V(TaskDeclaration, _) \
        V(VariableDeclaration, _) \
        V(TaskActionDeclaration, _) \
        V(TaskPropertyDeclaration, _) \
        V(BlockStatement, _) \
        V(MatchStatement, _) \
        V(LabeledStatement, _) \
        V(ForStatement, _) \
        V(WhileStatement, _) \
        V(WithStatement, _) \
        V(TryStatement, _) \
        V(ExceptStatement, _) \
        V(FinallyStatement, _) \
        V(BreakStatement, _) \
        V(ContinueStatement, _) \
        V(ReturnStatement, _) \
        V(PassStatement, _) \
        V(AssignStatement, _) \
        V(TypeTestExpression, _) \
        V(InvocationExpression, _) \
        V(ClosureExpression, _) \
        V(LiteralExpression, _)

        NODES_TO_CHECK(DEFINE_WALK)

#undef NODES_TO_CHECK

    public:
        explicit SemanticAnalyzer(Context& context);

        void Verify(ASTNode* node) { super::Walk(node); }
    }; // end class SemanticAnalyzer
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_ANALYSIS_SEMANTICANALYZER_H