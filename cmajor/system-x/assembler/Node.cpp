// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Node.hpp>

namespace cmsx::assembler {

Node::Node(NodeKind kind_, const SourcePos& sourcePos_) : kind(kind_), sourcePos(sourcePos_)
{
}

Node::~Node()
{
}

void Node::Write(CodeFormatter& formatter)
{
}

} // namespace cmsx::assembler
