/*
 * ASTVisitor.cpp
 * - Base class of Visitor pattern in AST.
 *
 * Copyright (c) 2019~2020 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/ASTVisitor.h>

void BuildScript::Parameters::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::TaskDeclaration::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClassDeclaration::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClassStaticMember::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClassMethod::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClassInit::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClassDeinit::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClassProperty::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClassOperator::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::FunctionDeclaration::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ImportDeclaration::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ExportDeclaration::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::VariableDeclaration::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ErrorStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::BlockStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::AssignmentStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::IfStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::MatchStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::LabeledStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ForStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::WhileStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::TryStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::RaiseStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::BreakStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ContinueStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ReturnStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::EmptyStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::AssertStatement::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ErrorExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::TernaryExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::BinaryExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::CastExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::UnaryExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::DefinedExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::MemberAccessExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::SubscriptExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::InvocationExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::LiteralExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::StringLiteral::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::MapExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ListExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::TupleExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::NamedTupleExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }

/*virtual*/ void BuildScript::ClosureExpression::Accept(BuildScript::ASTVisitor* visitor) const { visitor->Visit(this); }