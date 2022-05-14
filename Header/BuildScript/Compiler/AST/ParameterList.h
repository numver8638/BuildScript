/*
 * ParameterList.h
 * - Represents parameter list in every callables.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_PARAMETERLIST_H
#define BUILDSCRIPT_COMPILER_AST_PARAMETERLIST_H

#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Utils/TrailObjects.h>

namespace BuildScript {
    class Context; // Defined in <BuildScript/Compiler/Context.h>
    class Parameter; // Defined in <BuildScript/Compiler/AST/Declarations.h>

    /**
     * @brief Represents parameter list in every callables.
     */
    class ParameterList final : public ASTNode, TrailObjects<ParameterList, Parameter*, SourcePosition> {
        friend TrailObjects;

    private:
        SourcePosition m_open;
        SourcePosition m_ellipsis;
        SourcePosition m_close;
        size_t m_count;

        ParameterList(SourcePosition open, SourcePosition ellipsis, SourcePosition close, size_t count)
            : ASTNode(ASTKind::Parameters), m_open(open), m_ellipsis(ellipsis), m_close(close), m_count(count) {}

        // TrailObjects support.
        size_t GetTrailCount(OverloadToken<Parameter*>) const { return m_count; }
        size_t GetTrailCount(OverloadToken<SourcePosition>) const { return (m_count == 0) ? 0 : (m_count - 1); }

    public:
        /**
         * @brief Get a position of '('.
         * @return a @c SourcePosition representing where '(' positioned.
         */
        SourcePosition GetOpenParenPosition() const { return m_open; }

        /**
         * @brief Get a position of ')'.
         * @return a @c SourcePosition representing where ')' positioned.
         */
        SourcePosition GetCloseParenPosition() const { return m_close; }

        /**
         * @brief Get parameters of the parameter list.
         * @return a @c TrailIterator that iterates @c Parameter.
         */
        TrailIterator<Parameter*> GetParameters() const { return GetTrailObjects<Parameter*>(); }

        /**
         * @brief Get a count of parameters.
         * @return a count of parameters.
         */
        int GetParameterCount() const { return static_cast<int>(m_count); }

        /**
         * @brief Check parameter list has variadic arguments.
         * @return @c true if it has, otherwise @c false.
         */
        bool HasVariadicArgument() const { return (bool)m_ellipsis; }

        /**
         * @brief Get a position of '...'.
         * @return a @c SourcePosition representing where '...' positioned.
         */
        SourcePosition GetEllipsisPosition() const { return m_ellipsis; }

        static ParameterList*
        Create(Context& context, SourcePosition open, const std::vector<Parameter*>& params,
               const std::vector<SourcePosition>& commas, SourcePosition ellipsis, SourcePosition close);
    }; // end class ParameterList

    inline ParameterList* ASTNode::AsParameterList() {
        return IsParameters() ? static_cast<ParameterList*>(this) : nullptr; // NOLINT
    }

    inline const ParameterList* ASTNode::AsParameterList() const {
        return IsParameters() ? static_cast<const ParameterList*>(this) : nullptr; // NOLINT
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_PARAMETERLIST_H