// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_REGISTRY_INCLUDED
#define CMAJOR_WING_REGISTRY_INCLUDED
#include <wing/Wing.hpp>

namespace cmajor { namespace wing {

enum class RegistryValueKind
{
    regSz, regExpandSz
};

class WING_API RegistryKey
{
public:
    RegistryKey(HKEY key_);
    ~RegistryKey();
    bool HasValue(const std::string& name) const;
    std::string GetValue(const std::string& name) const;
    void SetValue(const std::string& name, const std::string& value, RegistryValueKind valueKind);
    void DeleteValue(const std::string& name);
    static RegistryKey CurrentUser(REGSAM access);
    static RegistryKey Open(HKEY predefinedKey, const std::string& subKey, REGSAM access);
private:
    HKEY key;
};

} } // cmajor::wing

#endif // CMAJOR_WING_REGISTRY_INCLUDED
