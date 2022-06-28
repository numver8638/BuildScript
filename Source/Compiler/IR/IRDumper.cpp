/*
 * IRDumper.cpp
 * - Print IR code.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRDumper.h>

#include <iostream>

#include <BuildScript/Compiler/IR/BasicBlock.h>
#include <BuildScript/Compiler/Symbol//Symbol.h>

using namespace BuildScript;

std::ostream& operator <<(std::ostream& OS, const Value& value) {
    if (value) {
        std::visit(overload {
            [&OS](std::monostate) { OS << "none"; },
            [&OS](std::string& val) { OS << "\"" << val << "\""; },
            [&OS](auto&& val) { OS << std::boolalpha << val; },
        }, *value);
    }
    else {
        OS << "INVALID_VALUE";
    }
    return OS;
}

std::ostream& operator <<(std::ostream& OS, IRValue value) {
    if (value == IRInvalidValue) {
        OS << "%<invalid>";
    }
    else {
        OS << "%" << static_cast<uint32_t>(value);
    };

    return OS;
}

std::ostream& operator <<(std::ostream& OS, IROpCode value) {
    return OS << IROpCodeToString(value);
}

std::ostream& operator <<(std::ostream& OS, IRTestKind value) {
    static std::string_view Table[] = {
        "GRATER",
        "GRATER_EQUAL",
        "LESS",
        "LESS_EQUAL",
        "EQUAL",
        "NOT_EQUAL",
        "EQUAL_TYPE",
        "NOT_EQUAL_TYPE",
        "CONTAIN",
        "NOT_CONTAIN"
    };
    return OS << Table[static_cast<size_t>(value)];
}

std::ostream& operator <<(std::ostream& OS, BasicBlock* value) {
    return OS << "@" << value->GetLabel();
}

std::ostream& operator <<(std::ostream& OS, Symbol* value) {
    if (value == nullptr) {
        OS << "<invalid symbol>";
    }
    else {
        OS << value->GetMangledName();
    }
    return OS;
}

void IRDumper::Dump(std::vector<IRCodeBlock>& codes) {
    for (auto& code : codes) {
        m_out << "IRCodeBlock \"" << code.GetName() << "\"" << std::endl;

        m_out << "ExceptHandlers:" << std::endl;
        for (auto& info : code.GetExceptHandlers()) {
            m_out << "    Range: " << info.Begin->GetLabel() << " ~ " << info.End->GetLabel() << std::endl;
            m_out << "    Handlers:" << std::endl;

            for (auto& [handler, type] : info.Handlers) {
                m_out << "        Type: ";
                m_out << (type == nullptr ? "<finally>" : type->GetMangledName());
                m_out << ", Begin: " << handler->GetLabel() << std::endl;
            }
        }

        for (auto* block : code.GetBlocks()) {
            Dump(*block);
        }

        m_out << std::endl;
    }
}

void IRDumper::Dump(BasicBlock& block) {
    m_out << "BasicBlock \"" << block.GetLabel() << "\":" << std::endl;

    m_out << "Predecessors:" << std::endl;
    for (auto* pred : block.GetPredecessors()) {
        m_out << "    - " << pred->GetLabel() << std::endl;
    }
    m_out << "Successors:" << std::endl;
    for (auto* succ : block.GetSuccessors()) {
        m_out << "    - " << succ->GetLabel() << std::endl;
    }
    m_out << "Defined Symbols:" << std::endl;
    for (auto& [symbol, value] : block.Defined) {
        m_out << "    - " << symbol->GetMangledName() << " -> " << value << std::endl;
    }
    m_out << "IR:" << std::endl;
    for (auto* stmt : block) {
        m_out << "    ";
        stmt->Accept(this);
    }
}

void IRDumper::Visit(IRLoadConstOp& node) {
    // %<reg> = LOAD_CONST #<index>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetValue() << std::endl;
}

void IRDumper::Visit(IRLoadSymbolOp& node) {
    // %<reg> = LOAD_SYMBOL #<index>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetSymbol() << std::endl;
}

void IRDumper::Visit(IRStoreSymbolOp& node) {
    // STORE_SYMBOL #<index>, %<reg>
    m_out << node.GetOp() << " " << node.GetSymbol() << ", " << node.GetValue() << std::endl;
}

void IRDumper::Visit(IRDeclareSymbolOp& node) {
    // DECLARE_SYMBOL #<index>, %<reg>
    m_out << node.GetOp() << " " << node.GetSymbol() << ", " << node.GetValue() << std::endl;
}

void IRDumper::Visit(IRBinaryOp& node) {
    // %<reg> = BINARY_OP %<reg>, %<reg>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetLeft() << ", " << node.GetRight() << std::endl;
}

void IRDumper::Visit(IRInplaceOp& node) {
    // %<reg> = INPLACE_BINARY_OP %<reg>, %<reg>
    m_out << node.GetReturnValue() << " = INPLACE_" << node.GetOp() << " " << node.GetTarget() << ", " << node.GetValue() << std::endl;
}

void IRDumper::Visit(IRUnaryOp& node) {
    // %<reg> = UNARY_OP %<reg>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetValue() << std::endl;
}

void IRDumper::Visit(IRDefinedOp& node) {
    // %<reg> = DEFINED #<index>, %<reg>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetID() << ", " << node.GetTarget() << std::endl;
}

void IRDumper::Visit(IRTestOp& node) {
    // %<reg> = TEST <FLAG> %<reg>, %<reg>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetKind() << " " << node.GetLeft() << ", " << node.GetRight() << std::endl;
}

void IRDumper::Visit(IRCallOp& node) {
    // %<reg> = CALL %<reg>, ...
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetTarget();

    for (auto arg : node.GetArguments()) {
        m_out << ", " << arg;
    }

    m_out << std::endl;
}

void IRDumper::Visit(IRInvokeOp& node) {
    // %<reg> = INVOKE %<reg>, ...
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetTarget() << ", " << node.GetMember();

    for (auto arg : node.GetArguments()) {
        m_out << ", " << arg;
    }

    m_out << std::endl;
}

void IRDumper::Visit(IRGetMemberOp& node) {
    // %<reg> = GET_MEMBER %<reg>, #<index>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetTarget() << ", " << node.GetMember() << std::endl;
}

void IRDumper::Visit(IRGetSubscriptOp& node) {
    // %<reg> = GET_SUBSCRIPT %<reg>, %<reg>
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " " << node.GetTarget() << ", " << node.GetIndex() << std::endl;
}

void IRDumper::Visit(IRSetMemberOp& node) {
    // SET_MEMBER %<reg>, #<index>, %<reg>
    m_out << node.GetOp() << " " << node.GetTarget() << ", " << node.GetMember() << ", " << node.GetValue() << std::endl;
}

void IRDumper::Visit(IRSetSubscriptOp& node) {
    // SET_SUBSCRIPT %<reg>, %<reg>, %<reg>
    m_out << node.GetOp() << " " << node.GetTarget() << ", " << node.GetIndex() << ", " << node.GetValue() << std::endl;
}

void IRDumper::Visit(IRBranchOp& node) {
    // BR <label>
    m_out << node.GetOp() << node.GetBranch() << std::endl;
}

void IRDumper::Visit(IRConditionalBranchOp& node) {
    // BR_COND %<reg>, <true>, <false>
    m_out << node.GetOp() << " " << node.GetCondition() << ", " << node.GetBranchOnTrue() << ", " << node.GetBranchOnFalse() << std::endl;
}

void IRDumper::Visit(IRJumpTableOp& node) {
    // JUMP_TABLE %<reg>, <default>, ...
    m_out << node.GetOp() << " " << node.GetCondition() << ", " << node.GetDefaultBlock();

    for (auto& [value, block] : node.GetCaseBlocks()) {
        m_out << ", <" << value << ", " << block << ">";
    }

    m_out << std::endl;
}

void IRDumper::Visit(IRReturnOp& node) {
    // RETURN %<reg>
    m_out << node.GetOp() << " " << node.GetReturnValue() << std::endl;
}

void IRDumper::Visit(IRRaiseOp& node) {
    // RAISE %<reg>
    m_out << node.GetOp() << " " << node.GetThrowable() << std::endl;
}

void IRDumper::Visit(IRAssertOp& node) {
    // ASSERT %<reg>, %<reg>
    m_out << node.GetOp() << " " << node.GetCondition() << ", " << node.GetMessage() << std::endl;
}

void IRDumper::Visit(IRSelectOp& node) {
    // %<reg> = SELECT %<reg>, ...
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " ";

    for (auto arg : node.GetTargets()) {
        m_out <<  arg << ", ";
    }

    m_out << std::endl;
}

void IRDumper::Visit(IRMakeOp& node) {
    // %<reg> = MAKE_OP %<reg>, ...
    m_out << node.GetReturnValue() << " = " << node.GetOp() << " ";

    for (auto arg : node.GetArguments()) {
        m_out <<  arg << ", ";
    }

    m_out << std::endl;
}

void IRDumper::Visit(IRImportOp& node) {
    // IMPORT %<reg>
    m_out << node.GetOp() << " " << node.GetPath() << std::endl;
}

void IRDumper::Visit(IRExportOp& node) {
    // EXPORT #<name>
    m_out << node.GetOp() << " " << node.GetSymbol() << std::endl;
}