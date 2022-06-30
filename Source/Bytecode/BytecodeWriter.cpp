/*
 * BytecodeWriter.cpp
 * - Create bytecode.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Bytecode/BytecodeWriter.h>

#include <array>
#include <algorithm>
#include <iterator>

#include <BuildScript/Assert.h>
#include <BuildScript/Types/Code.h>

using namespace BuildScript;

std::vector<uint8_t>& operator <<(std::vector<uint8_t>& vec, OpCode val) {
    vec.push_back(static_cast<uint8_t>(val));
    return vec;
}

std::vector<uint8_t>& operator <<(std::vector<uint8_t>& vec, uint8_t val) {
    vec.push_back(val);
    return vec;
}

template <typename T>
std::array<uint8_t, sizeof(T)> Arraify(T val) {
    std::array<uint8_t, sizeof(T)> array{};

    std::memcpy(array.data(), &val, sizeof(T));

    return std::move(array);
}

std::vector<uint8_t>& operator <<(std::vector<uint8_t>& vec, uint16_t val) {
    auto b = Arraify(val);
    std::copy(std::begin(b), std::end(b), std::back_inserter(vec));

    return vec;
}

std::vector<uint8_t>& operator <<(std::vector<uint8_t>& vec, int16_t val) {
    auto b = Arraify(val);
    std::copy(std::begin(b), std::end(b), std::back_inserter(vec));

    return vec;
}

std::vector<uint8_t>& operator <<(std::vector<uint8_t>& vec, uint32_t val) {
    auto b = Arraify(val);
    std::copy(std::begin(b), std::end(b), std::back_inserter(vec));

    return vec;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << reg;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint16_t index) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << index;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint16_t index1, uint16_t index2) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << index1 << index2;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg, int16_t val) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << reg << val;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg, uint16_t index) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << reg << index;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg1, uint8_t reg2) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << reg1 << reg2;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg1, uint8_t reg2, uint16_t index) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << reg1 << reg2 << index;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg1, uint8_t reg2, uint8_t reg3) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << reg1 << reg2 << reg3;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, TestOpCode test, uint8_t reg1, uint8_t reg2, uint8_t reg3) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    m_buffer << op << static_cast<uint8_t>(test) << reg1 << reg2 << reg3;
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, Label* label) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    assert((op == OpCode::Br) && "only br call this method.");

    m_buffer << op;

    m_refs.emplace_back(label, /*absolute=*/true, m_buffer.size());

    m_buffer << uint32_t(0); // must be updated later.
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg, Label* label) {
    if (pos) { m_lines.emplace_back(pos, m_buffer.size()); }
    assert((op == OpCode::BrTrue || op == OpCode::BrFalse) && "only brcond call this method.");

    m_buffer << op << reg;

    m_refs.emplace_back(label, /*absolute=*/false, m_buffer.size());

    m_buffer << uint16_t(0); // must be updated later
}

void BytecodeWriter::Write(SourcePosition pos, OpCode op, uint8_t reg, Label* _default, const JumpTable& jumptable) {
    assert((op == OpCode::JumpTable) && "only jumptable call this method.");

    m_buffer << op << reg << uint16_t(jumptable.size());

    m_refs.emplace_back(_default, /*absolute=*/true, m_buffer.size());
    m_buffer << uint32_t(0);


    for (auto& [val, addr]: jumptable) {
        m_buffer << val;

        m_refs.emplace_back(addr, /*absolute=*/true, m_buffer.size());

        m_buffer << uint32_t(0);
    }
}

void BytecodeWriter::RegisterLabel(BasicBlock* block) {
    auto [it, _] = m_labels.emplace(block, Label(block));

    it->second.Begin = m_buffer.size();
}

BytecodeWriter::Label* BytecodeWriter::GetLabel(BasicBlock* block) {
    auto [it, _] = m_labels.emplace(block, Label(block));

    return &(it->second);
}

void BytecodeWriter::EndLabel(BasicBlock* block) {
    auto it = m_labels.find(block);

    assert(it != m_labels.end() && "cannot end unregistered label.");

    it->second.End = m_buffer.size() - 1;
}

std::vector<uint8_t> BytecodeWriter::Build() {
    for (auto& ref : m_refs) {
        if (ref.AbsoluteAddress) {
            if (ref.Reference->Begin > std::numeric_limits<uint32_t>::max()) {
                // report an error: interpreter limit: absolute branch exceeds UINT32_MAX
            }

            auto b = Arraify<uint32_t>(ref.Reference->Begin);
            std::copy(std::begin(b), std::end(b), m_buffer.begin() + ref.Position);
        }
        else {
            auto diff = (int64_t(ref.Reference->Begin) - int64_t(ref.Position)) - 2;

            if ((diff < std::numeric_limits<int16_t>::min()) || (std::numeric_limits<int16_t>::max() < diff)) {
                // report an error: interpreter limit: relative branch exceeds min(int16_t) ~ max(int16_t)
            }

            auto b = Arraify<int16_t>(static_cast<int16_t>(diff));
            std::copy(std::begin(b), std::end(b), m_buffer.begin() + ref.Position);
        }
    }

    return std::move(m_buffer);
}

std::vector<LineInfo> BytecodeWriter::GetLineInfo() {
    std::vector<LineInfo> lines;
    for (auto& [pos, addr] : m_lines) {
        lines.emplace_back(uint32_t(addr), pos.Line, pos.Column);
    }

    return std::move(lines);
}