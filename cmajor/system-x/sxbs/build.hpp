// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SXBS_BUILD_INCLUDED
#define SXBS_BUILD_INCLUDED
#include <stdint.h>
#include <string>
#include <memory>

namespace sxbs {

class ServerThread;

enum class Config : int32_t
{
    none = 0,
    debug = 1 << 0,
    release = 1 << 1,
    both = debug | release
};

inline Config operator|(Config left, Config right)
{
    return Config(static_cast<int32_t>(left) | static_cast<int32_t>(right));
}

inline Config operator&(Config left, Config right)
{
    return Config(static_cast<int32_t>(left) & static_cast<int32_t>(right));
}

inline Config operator~(Config config)
{
    return Config(~static_cast<int32_t>(config));
}

void Build(ServerThread* serverThread, const std::string& project, Config config);

} // sxbs

#endif // SXBS_BUILD_INCLUDED

