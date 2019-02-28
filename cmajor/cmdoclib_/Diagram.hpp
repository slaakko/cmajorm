// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_DIAGRAM_INCLUDED
#define CMAJOR_CMDOCLIB_DIAGRAM_INCLUDED
#include <cmajor/util/CodeFormatter.hpp>
#include <vector>

namespace cmajor { namespace cmdoclib {

struct DiagramNode
{
    DiagramNode(int index_, int level_, const std::u32string& name_, bool subject_, bool hasDerivedNodes_, const std::u32string& link_);
    void Write(cmajor::util::CodeFormatter& formatter);
    int index;
    int level;
    std::u32string name;
    bool subject;
    bool hasDerivedNodes;
    std::u32string link;
};

struct DiagramNodeByLevelAndName
{
    bool operator()(const DiagramNode& left, const DiagramNode& right) const;
};

void GenerateDiagramCode(std::vector<DiagramNode>& diagramNodes, const std::string& drawFunctionName, const std::string& svgId, const std::string& scriptFilePath);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_DIAGRAM_INCLUDED
