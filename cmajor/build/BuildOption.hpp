// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_OPTION_INCLUDED
#define CMAJOR_BUILD_OPTION_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <stdint.h>
#include <string>

namespace cmajor { namespace build { 

enum class BuildOptions : int8_t
{
    none = 0,
    verbose = 1 << 0,
    debug = 1 << 1,
    force = 1 << 2,
    only = 1 << 3,
    all = 1 << 4,
    push = 1 << 5,
    build = 1 << 6
};

inline BuildOptions operator|(BuildOptions left, BuildOptions right)
{
    return BuildOptions(int8_t(left) | int8_t(right));
}

inline BuildOptions operator&(BuildOptions left, BuildOptions right)
{
    return BuildOptions(int8_t(left) & int8_t(right));
}

extern BuildOptions buildOptions;
extern std::string buildConfig;
extern std::string buildToolChain;

BUILD_API std::string BoolStr(bool value);

BUILD_API bool GetBuildOption(BuildOptions option);

class BUILD_API BuildOptionSetter
{
public:
    void SetOption(BuildOptions option);
    void SetConfigOption(const std::string& config);
    void SetToolChainOption(const std::string& toolChain);
};

} } // namespace cmajor::build;

#endif // CMAJOR_BUILD_OPTION_INCLUDED

