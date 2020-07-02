/*
 * NodeWriter.h
 * - Text writer for AST nodes.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_NODEWRITER_H
#define BUILDSCRIPT_NODEWRITER_H

#include <functional>
#include <string>

#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Compiler/StringRef.h>

namespace BuildScript {
    /**
     * @brief Handler delegates writing text in NodeWriter.
     */
    using Writer = std::function<void(const std::string&)>;

    /**
     * @brief Represent end-of-line in NodeWriter.
     */
    constexpr struct {} EOL;

    /**
     * @brief Text writer for AST nodes.
     */
    class NodeWriter final {
        friend class IndentRAII;

    private:
        Writer m_writer;
        const int m_tabsize;
        int m_indent = 0;
        bool m_newline = false;

        void Indent() {
            ++m_indent;
        }

        void Dedent() {
            if (m_indent > 0) --m_indent;
        }

        void WriteString(const std::string& str);

    public:
        NodeWriter(Writer writer, int tabsize = 2)
            : m_writer(std::move(writer)), m_tabsize(tabsize) {}


        NodeWriter& operator <<(const SourcePosition&);
        NodeWriter& operator <<(const StringRef&);

        NodeWriter& operator <<(const char* str) {
            WriteString(str);
            return *this;
        }

        NodeWriter& operator <<(const std::string& str) {
            WriteString(str);
            return *this;
        }

        NodeWriter& operator <<(bool b) {
            WriteString(b ? "true" : "false");
            return *this;
        }

        NodeWriter& operator <<(decltype(EOL)&) {
            m_newline = true;
            return *this;
        }
    };

    /**
     * @brief Proxy class for auto indentation.
     */
    class IndentRAII final {
    private:
        NodeWriter& m_writer;

    public:
        explicit IndentRAII(NodeWriter& writer)
            : m_writer(writer) {
            m_writer.Indent();
        }

        ~IndentRAII() {
            m_writer.Dedent();
        }
    }; // end class IndentRAII
} // end namespace BuildScript

#endif //BUILDSCRIPT_NODEWRITER_H