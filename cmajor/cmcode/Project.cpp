// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/Project.hpp>

namespace cmcode {

ProjectData::ProjectData(sngcm::ast::Project* project_) : project(project_), breakpointCollection()
{
}

} // namespace cmcode
