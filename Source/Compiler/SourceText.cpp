/*
 * SourceText.cpp
 * - Represent a source code.
 * 
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/SourceText.h>

#include <boost/filesystem.hpp>

using namespace BuildScript;

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

inline size_t CalculateBOM(const char* begin, const char* end, const std::vector<char>& bom) {
    if (end - begin < bom.size())
        return 0;
    
    for (auto b : bom)
        if (*begin++ != b)
            return 0;
    
    return bom.size();
}

SourceText::SourceText(const std::string& filename, Encoding& encoding, int tabsize)
    : m_file(boost::none), m_filename(filename), m_encoding(encoding), m_tabsize(tabsize),
      m_begin(nullptr), m_current(nullptr), m_end(nullptr),
      m_line(1), m_column(1) {
    fs::path path(filename);

    if (!fs::exists(path) || !fs::is_regular_file(path))
        throw std::ios_base::failure("file not exist or not a file");

    if (fs::file_size(path) != 0) {
        m_file = io::mapped_file(filename, io::mapped_file::readonly);

        m_begin = m_file->const_begin();
        m_end = m_file->const_end();
        m_current = m_begin + CalculateBOM(m_begin, m_end, encoding.GetBOM());
    }
}

SourceText::SourceText(const char* begin, const char* end, int tabsize)
    : m_file(boost::none), m_filename(InternalSourceName), m_encoding(Encoding::UTF8()), m_tabsize(tabsize),
      m_begin(begin), m_current(begin), m_end(end),
      m_line(1), m_column(1) {}

int SourceText::PeekChar() const {
    int _ = 0;
    return (m_current < m_end) ? m_encoding.DecodeChar(m_current, m_end, _) : EndOfFile;
}

void SourceText::Consume() {
    if (m_current < m_end) {
        int length = 0;
        int ch = m_encoding.DecodeChar(m_current, m_end, length);

        m_current += length;

        // Update line & column.
        switch (ch) {
            case u'\r':
                if (m_encoding.DecodeChar(m_current, m_end, length) == u'\n') {
                    m_current += length;
                }
                /*[[fallthrough]]*/

            case u'\n':
                m_line++;
                m_column = 0;
                break;

            case u'\t':
                m_column += m_tabsize - (m_column % m_tabsize);
                break;

            default:
                // Do nothing.
                break;
        }

        ++m_column;
    }
}