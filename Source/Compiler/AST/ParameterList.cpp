/*
 * Parameters.cpp
 * - Represents parameter list in AST.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/Compiler/AST/ParameterList.h>

#include <BuildScript/Compiler/Context.h>

using namespace BuildScript;

ParameterList*
ParameterList::Create(Context& context, SourcePosition open, const std::vector<Parameter*>& params,
                   const std::vector<SourcePosition>& commas, SourcePosition ellipsis, SourcePosition close) {
    assert((params.size() < 2 && commas.empty()) || ((params.size() - 1) == commas.size())
            && "count of parameter names and commas does not match.");

    auto trailSize = GetTrailSize(params.size(), commas.size());
    auto* node = new (context.GetAllocator(), trailSize) ParameterList(open, ellipsis, close, params.size());

    node->SetTrailObjects(params.data(), params.size());
    node->SetTrailObjects(commas.data(), commas.size());

    return node;
}