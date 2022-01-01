// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_ENVIRONMENT_INCLUDED
#define CMAJOR_WING_ENVIRONMENT_INCLUDED
#include <wing/Registry.hpp>

namespace cmajor { namespace wing {

WING_API bool HasSystemEnvironmentVariable(const std::string& name);

WING_API std::string GetSystemEnvironmentVariable(const std::string& name);

WING_API void SetSystemEnvironmentVariable(const std::string& name, const std::string& value, RegistryValueKind valueKind);

WING_API void DeleteSystemEnvironmentVariable(const std::string& name);

WING_API bool HasPathDirectory(const std::string& directory);

WING_API void AppendPathDirectory(const std::string& directory);

WING_API void RemovePathDirectory(const std::string& directory);

WING_API void BroadcastEnvironmentChangedMessage();

} } // cmajor::wing

#endif // CMAJOR_WING_ENVIRONMENT_INCLUDED
