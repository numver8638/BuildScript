/*
 * BytecodeWriter.h
 * - Create bytecode.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#ifndef BUILDSCRIPT_BYTECODE_BYTECODEWRITER_H
#define BUILDSCRIPT_BYTECODE_BYTECODEWRITER_H

#include <cstdint>
#include <vector>
#include <map>

#include <BuildScript/Compiler/SourcePosition.h>
#include <BuildScript/Bytecode/Opcode.h>

namespace BuildScript {
    struct LineInfo;
    class BasicBlock; // defined in <BuildScript/Compiler/IR/BasicBlock>
    class ScriptCode; // defined in <BuildScript/Types/Code.h>

    /**
     * @brief Create bytecode.
     */
    class BytecodeWriter final {
    public:
        struct Label {
            BasicBlock* Block;
            size_t Begin = 0;
            size_t End = 0;


            Label(BasicBlock* block)
                : Block(block) {}
        }; // end class Label

        struct LabelRef {
            Label* Reference;
            bool AbsoluteAddress;
            size_t Position;

            LabelRef(Label* ref, bool absolute, size_t pos)
                : Reference(ref), AbsoluteAddress(absolute), Position(pos) {}
        }; // end class LabelRef

        using JumpTable = std::vector<std::pair<uint16_t, Label*>>;

    private:
        std::vector<uint8_t> m_buffer;
        std::map<BasicBlock*, Label> m_labels;
        std::vector<LabelRef> m_refs;
        std::vector<std::tuple<SourcePosition, size_t>> m_lines;

    public:
        /**
         * @brief Create and register new label.
         * @param block a @c BasicBlock to make a label.
         */
        void RegisterLabel(BasicBlock* block);

        /**
         * @brief Get a label.
         * @param block a @c BasicBlock corresponds to the label.
         * @return a @c Label corresponds to @c block.
         */
        Label* GetLabel(BasicBlock* block);

        /**
         * @brief Mark end of label.
         * @param block a @c BasicBlock corresponds to the label.
         */
        void EndLabel(BasicBlock* block);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         */
        void Write(SourcePosition pos, OpCode op);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg a register index.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param index a constant pool index.
         */
        void Write(SourcePosition pos, OpCode op, uint16_t index);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param index1 first constant pool index.
         * @param index2 second constant pool index.
         */
        void Write(SourcePosition pos, OpCode op, uint16_t index1, uint16_t index2);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg a register index
         * @param val signed integer value.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg, int16_t val);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg a register index.
         * @param index a constant pool index.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg, uint16_t index);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg1 first register index.
         * @param reg2 second register index.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg1, uint8_t reg2);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg1 first register index.
         * @param reg2 second register index.
         * @param index a constant pool index.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg1, uint8_t reg2, uint16_t index);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg1 first register index.
         * @param reg2 second register index.
         * @param reg3 third register index.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg1, uint8_t reg2, uint8_t reg3);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param test an @c TestOpCode.
         * @param reg1 first register index.
         * @param reg2 second register index.
         * @param reg3 third register index.
         */
        void Write(SourcePosition pos, OpCode op, TestOpCode test, uint8_t reg1, uint8_t reg2, uint8_t reg3);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param label a @c Label where jumps into.
         */
        void Write(SourcePosition pos, OpCode op, Label* label);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg a register index.
         * @param label a @c Label where jumps into.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg, Label* label);

        /**
         * @brief Write a bytecode.
         * @param pos a @c SourcePosition where the bytecode comes from the source.
         * @param op an @c OpCode.
         * @param reg a register index.
         * @param _default  a @c Label where jumps into when a default condition.
         * @param jumptable a @c JumpTable representing dispatch table.
         */
        void Write(SourcePosition pos, OpCode op, uint8_t reg, Label* _default, const JumpTable& jumptable);

        /**
         * @brief Complete bytecode buffer and return the buffer.
         * @return a @c std::vector representing bytecodes.
         */
        std::vector<uint8_t> Build();

        /**
         * @brief Get lines information.
         * @return a @c std::vector representing lines information table.
         */
        std::vector<LineInfo> GetLineInfo();
    }; // end class BytecodeWriter
} // end namespace BuildScript

#endif // BUILDSCRIPT_BYTECODE_BYTECODEWRITER_H