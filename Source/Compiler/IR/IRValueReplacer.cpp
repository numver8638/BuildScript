/*
 * IRValueReplacer.cpp
 * - Replaces IRValue in IRStatement.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/IR/IRValueReplacer.h>

#include <algorithm>

#include <BuildScript/Compiler/IR/BasicBlock.h>

using namespace BuildScript;

void IRValueReplacer::Visit(IRLoadConstOp& op) {
    // do nothing
}

void IRValueReplacer::Visit(IRLoadSymbolOp& op) {
    // do nothing
}

void IRValueReplacer::Visit(IRStoreSymbolOp& op) {
    if (op.GetValue() == m_from) {
        op.SetValue(m_to);
    }
}

void IRValueReplacer::Visit(IRDeclareSymbolOp& op) {
    if (op.GetValue() == m_from) {
        op.SetValue(m_to);
    }
}

void IRValueReplacer::Visit(IRBinaryOp& op) {
    if (op.GetLeft() == m_from) {
        op.SetLeft(m_to);
    }

    if (op.GetRight() == m_from) {
        op.SetRight(m_to);
    }
}

void IRValueReplacer::Visit(IRInplaceOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetTarget(m_to);
    }

    if (op.GetValue() == m_from) {
        op.SetValue(m_to);
    }
}

void IRValueReplacer::Visit(IRUnaryOp& op) {
    if (op.GetValue() == m_from) {
        op.SetValue(m_to);
    }
}

void IRValueReplacer::Visit(IRDefinedOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetTarget(m_to);
    }
}

void IRValueReplacer::Visit(IRTestOp& op) {
    if (op.GetLeft() == m_from) {
        op.SetLeft(m_to);
    }

    if (op.GetRight() == m_from) {
        op.SetRight(m_to);
    }
}

void IRValueReplacer::Visit(IRCallOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetTarget(m_to);
    }

    std::replace(op.GetArguments().begin(), op.GetArguments().end(), m_from, m_to);
}

void IRValueReplacer::Visit(IRInvokeOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetTarget(m_to);
    }

    std::replace(op.GetArguments().begin(), op.GetArguments().end(), m_from, m_to);
}

void IRValueReplacer::Visit(IRGetMemberOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetTarget(m_to);
    }
}

void IRValueReplacer::Visit(IRGetSubscriptOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetTarget(m_to);
    }

    if (op.GetIndex() == m_from) {
        op.SetTarget(m_to);
    }
}

void IRValueReplacer::Visit(IRSetMemberOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetTarget(m_to);
    }

    if (op.GetValue() == m_from) {
        op.SetValue(m_to);
    }
}

void IRValueReplacer::Visit(IRSetSubscriptOp& op) {
    if (op.GetTarget() == m_from) {
        op.SetValue(m_to);
    }

    if (op.GetIndex() == m_from) {
        op.SetIndex(m_to);
    }

    if (op.GetValue() == m_from) {
        op.SetValue(m_to);
    }
}

void IRValueReplacer::Visit(IRBranchOp& op) {
    // do nothing
}

void IRValueReplacer::Visit(IRConditionalBranchOp& op) {
    if (op.GetCondition() == m_from) {
        op.SetCondition(m_to);
    }
}

void IRValueReplacer::Visit(IRJumpTableOp& op) {
    if (op.GetCondition() == m_from) {
        op.SetCondition(m_to);
    }
}

void IRValueReplacer::Visit(IRReturnOp& op) {
    if (op.GetReturnValue() == m_from) {
        op.SetReturnValue(m_to);
    }
}

void IRValueReplacer::Visit(IRRaiseOp& op) {
    if (op.GetThrowable() == m_from) {
        op.SetThrowable(m_to);
    }
}

void IRValueReplacer::Visit(IRAssertOp& op) {
    if (op.GetCondition() == m_from) {
        op.SetCondition(m_to);
    }

    if (op.GetMessage() == m_from) {
        op.SetMessage(m_to);
    }
}

void IRValueReplacer::Visit(IRSelectOp& op) {
    std::replace(op.GetTargets().begin(), op.GetTargets().end(), m_from, m_to);
}

void IRValueReplacer::Visit(IRMakeOp& op) {
    std::replace(op.GetArguments().begin(), op.GetArguments().end(), m_from, m_to);
}

void IRValueReplacer::Visit(IRImportOp& op) {
    if (op.GetPath() == m_from) {
        op.SetPath(m_to);
    }
}

void IRValueReplacer::Visit(IRExportOp& op) {
    // do nothing
}

void IRValueReplacer::Replace(BasicBlock* block) {
    for (auto* op : *block) {
        op->Accept(this);
    }
}