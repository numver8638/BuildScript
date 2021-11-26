/*
 * Parameters.h
 * - Represents parameter list in every callables.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_COMPILER_AST_PARAMETERS_H
#define BUILDSCRIPT_COMPILER_AST_PARAMETERS_H

#include <vector>

#include <BuildScript/Compiler/AST/ASTNode.h>
#include <BuildScript/Compiler/Identifier.h>
#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Utils/TrailObjects.h>

namespace BuildScript {
    class Context; // Defined in <BuildScript/Compiler/Context.h>

    /**
     * @brief Represents parameter list in every callables.
     */
    class Parameters final : public ASTNode, TrailObjects<Parameters, Identifier, SourcePosition> {
        friend TrailObjects;

    private:
        SourcePosition m_open;
        SourcePosition m_ellipsis;
        SourcePosition m_close;
        size_t m_count;

        Parameters(SourceRange range, SourcePosition open, SourcePosition ellipsis, SourcePosition close, size_t count)
            : ASTNode(ASTKind::Parameters, range), m_open(open), m_ellipsis(ellipsis), m_close(close),
              m_count(count) {}

        // TrailObjects support.
        size_t GetTrailCount(OverloadToken<Identifier>) const { return m_count; }
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

        TrailIterator<const Identifier> GetParameterNames() const { return GetTrailObjects<Identifier>(); }

        /**
         * @brief Get a count of parameters.
         * @return a count of parameters.
         */
        size_t GetParameterCount() const { return m_count; }

        const Identifier& GetParameterNameAt(size_t index) const { return At<Identifier>(index); }

        SourcePosition GetCommaPositionAt(size_t index) const { return At<SourcePosition>(index); }

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

        static Parameters*
        Create(Context& context, SourcePosition open, const std::vector<Identifier>& params,
               const std::vector<SourcePosition>& commas, SourcePosition ellipsis, SourcePosition close);
    }; // end class Parameters

    inline const Parameters* ASTNode::AsParameters() const {
        return IsParameters() ? static_cast<const Parameters*>(this) : nullptr; // NOLINT
    }
} // end namespace BuildScript

#endif // BUILDSCRIPT_COMPILER_AST_PARAMETERS_H