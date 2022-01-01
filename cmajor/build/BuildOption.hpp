// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_OPTION_INCLUDED
#define CMAJOR_BUILD_OPTION_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <stdint.h>
#include <string>

namespace cmajor { namespace build { 

enum class BuildOptions : int16_t
{
    none = 0,
    verbose = 1 << 0,
    messages = 1 << 1,
    debug = 1 << 2,
    force = 1 << 3,
    only = 1 << 4,
    all = 1 << 5,
    push = 1 << 6,
    build = 1 << 7,
    rebuild = 1 << 8
};

inline BuildOptions operator|(BuildOptions left, BuildOptions right)
{
    return BuildOptions(int16_t(left) | int16_t(right));
}

inline BuildOptions operator&(BuildOptions left, BuildOptions right)
{
    return BuildOptions(int16_t(left) & int16_t(right));
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

