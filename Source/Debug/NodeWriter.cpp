/*
 * NodeWriter.cpp
 * - Text writer for AST nodes.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Debug/NodeWriter.h>

using namespace BuildScript;

void NodeWriter::WriteString(const std::string &str) {
    std::string output;

    if (m_newline) {
        output.append(1, '\n');
    }

    output.append(m_tabsize * m_indent, ' ').append(str);

    m_writer(output);
}

NodeWriter& NodeWriter::operator <<(const StringRef& ref) {
    if (ref)
        WriteString(ref);
    else
        WriteString("<invalid StringRef>");

    return *this;
}

NodeWriter& NodeWriter::operator <<(const SourcePosition& pos) {
    std::string str;

    if (pos.IsValid()) {
        str.append(std::to_string(pos.Line)).append(":").append(std::to_string(pos.Column));
    } else {
        str.append("<invalid position>");
    }

    WriteString(str);

    return *this;
}