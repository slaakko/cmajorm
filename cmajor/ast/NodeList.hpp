// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_NODE_LIST_INCLUDED
#define CMAJOR_AST_NODE_LIST_INCLUDED
#include <cmajor/ast/AstWriter.hpp>
#include <cmajor/ast/AstReader.hpp>

namespace cmajor { namespace ast {

template<typename T>
class NodeList
{
public:
    int Count() const
    {
        return static_cast<int>(nodes.size());
    }
    T* operator[](int index) const
    {
        return nodes[index].get();
    }
    void Add(T* node)
    {
        nodes.push_back(std::unique_ptr<T>(node));
    }
    void SetParent(Node* parent)
    {
        for (const std::unique_ptr<T>& node : nodes)
        {
            node->SetParent(parent);
        }
    }
    void Write(AstWriter& writer)
    {
        uint32_t n = static_cast<uint32_t>(nodes.size());
        writer.GetBinaryWriter().WriteULEB128UInt(n);
        for (uint32_t i = 0; i < n; ++i)
        {
            writer.Write(nodes[i].get());
        }
    }
    void Read(AstReader& reader)
    {
        uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
        for (uint32_t i = 0; i < n; ++i)
        {
            Node* node = reader.ReadNode();
            T* asTNode = dynamic_cast<T*>(node);
            Assert(asTNode, "wrong node type");
            nodes.push_back(std::unique_ptr<T>(asTNode));
        }
    }
    std::vector<std::unique_ptr<T>>& Nodes() { return nodes; }
private:
    std::vector<std::unique_ptr<T>> nodes;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_NODE_LIST_INCLUDED
