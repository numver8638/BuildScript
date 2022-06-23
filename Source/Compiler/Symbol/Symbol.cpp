/*
 * Symbol.cpp
 * - Represents declared identifier in the source.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/Symbol/Symbol.h>

#include <sstream>

#include <fmt/format.h>

using namespace BuildScript;

std::string UndeclaredSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "external variable '" << GetName() << "'";

    return stream.str();
}

std::string VariableSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    static const std::string_view table[] = {
        "global variable",
        "variable",
        "exported variable",
        "parameter",
        "caught exception variable",
        "implicit variable"
    };

    stream << table[static_cast<size_t>(m_type)] << " '" << GetName() << "'";

    return stream.str();
}

static auto SelfSymbol = VariableSymbol("<self>", SourcePosition(), VariableType::Implicit, /*readonly=*/false);
static auto SuperSymbol = VariableSymbol("<super>", SourcePosition(), VariableType::Implicit, /*readonly=*/false);

// static
VariableSymbol* VariableSymbol::GetSelf() {
    return &SelfSymbol;
}

// static
VariableSymbol* VariableSymbol::GetSuper() {
    return &SuperSymbol;
}

std::string BoundedLocalSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "bounded local variable '" << GetName() << "'";

    return stream.str();
}

std::string BoundedLocalSymbol::GetMangledName() const {
    std::stringstream stream;

    stream << "bounded$" << m_orig->GetMangledName();

    return stream.str();
}

std::string TypeSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "type '" << GetName() << "'";

    return stream.str();
}

std::string FunctionSymbol::GetMangledName() const {
    std::stringstream stream;

    stream << GetName() << ArgumentSeparator << GetArgumentCount();

    if (HasVariadicArgument()) {
        stream << VariadicSuffix;
    }

    return stream.str();
}

std::string FunctionSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "function '" << GetName() << "'";

    return stream.str();
}

std::string ClassSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "class '" << GetName() << "'";

    return stream.str();
}

std::string TaskSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "task '" << GetName() << "'";

    return stream.str();
}

std::string FieldSymbol::GetMangledName() const {
    std::stringstream stream;

    stream << m_owner->GetName() << MemberSeparator << GetName();

    return stream.str();
}

std::string FieldSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "field '" << GetName();

    return stream.str();
}

std::string MethodSymbol::GetMangledName() const {
    std::stringstream stream;

    stream << m_owner->GetName() << MemberSeparator << GetName() << ArgumentSeparator << GetArgumentCount();

    if (HasVariadicArgument()) {
        stream << VariadicSuffix;
    }

    return stream.str();
}

std::string MethodSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    if (GetName() == ClassInitializerName) {
        stream << "class initializer";
    }
    else if (GetName() == InitializerName) {
        stream << "initializer";
    }
    else if (GetName() == DeinitializerName) {
        stream << "deinitializer";
    }
    else {
        stream << "method '" << GetName() << "'";
    }

    return stream.str();
}

std::string PropertySymbol::GetMangledName() const {
    std::stringstream stream;

    stream << m_owner->GetName() << MemberSeparator << GetName();

    return stream.str();
}

std::string PropertySymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "property '" << GetName() << "'";

    return stream.str();
}

// static
unsigned ClosureSymbol::m_counter = 0;

ClosureSymbol::ClosureSymbol(SourcePosition pos)
    : Symbol(SymbolType::Closure, fmt::format("<closure#{0}>", m_counter++), pos) {}

std::string ClosureSymbol::GetDescriptiveName() const {
    std::stringstream stream;

    stream << "closure '" << GetName() << "'";

    return stream.str();
}