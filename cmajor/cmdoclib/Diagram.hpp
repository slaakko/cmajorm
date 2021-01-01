// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_DIAGRAM_INCLUDED
#define CMAJOR_CMDOCLIB_DIAGRAM_INCLUDED
#include <cmajor/cmdoclib/CmDocLibApi.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <vector>

namespace cmajor { namespace cmdoclib {

struct CMDOCLIB_API DiagramNode
{
    DiagramNode(int index_, int level_, const std::u32string& name_, bool subject_, bool hasDerivedNodes_, const std::u32string& link_);
    void Write(soulng::util::CodeFormatter& formatter);
    int index;
    int level;
    std::u32string name;
    bool subject;
    bool hasDerivedNodes;
    std::u32string link;
};

struct CMDOCLIB_API DiagramNodeByLevelAndName
{
    bool operator()(const DiagramNode& left, const DiagramNode& right) const;
};

CMDOCLIB_API void GenerateDiagramCode(std::vector<DiagramNode>& diagramNodes, const std::string& drawFunctionName, const std::string& svgId, const std::string& scriptFilePath);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_DIAGRAM_INCLUDED
