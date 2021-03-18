// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_BUILD_INCLUDED
#define CMCODE_BUILD_INCLUDED
#include <string>

namespace cmcode {

enum class BuildRequestKind : int
{
    none = 0, build = 1 << 0, clean = 1 << 1, rebuild = 1 << 2, buildDependencies = 1 << 3
};

inline BuildRequestKind operator|(BuildRequestKind left, BuildRequestKind right)
{
    return BuildRequestKind(int(left) | int(right));
}

inline BuildRequestKind operator&(BuildRequestKind left, BuildRequestKind right)
{
    return BuildRequestKind(int(left) & int(right));
}

inline BuildRequestKind operator~(BuildRequestKind flags)
{
    return BuildRequestKind(~int(flags));
}

void StartBuild(const std::string& backend, const std::string& config, const std::string& filePath, BuildRequestKind requestKind);
void StopBuild();

} // namespace cmcode

#endif // CMCODE_BUILD_INCLUDED
