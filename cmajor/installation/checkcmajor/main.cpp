// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <winggui/message_dialog.hpp>
#include <wing/InitDone.hpp>
#include <wing/Wing.hpp>
#include <wing/Installation.hpp>
#include <wing/Registry.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <iostream>
#include <stdexcept>

using namespace wing;
using namespace wingstall::winggui;

struct InitDone
{
    InitDone(HINSTANCE instance)
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        wing::Init(instance);
    }
    ~InitDone()
    {
        wing::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

bool SearchCmajor(std::string& displayName, std::string& cmajorKeyPath)
{
    std::vector<std::string> keyPaths = GetInstalledSoftwareRegistryKeyPaths();
    for (const std::string& keyPath : keyPaths)
    {
        RegistryKey key = RegistryKey::Open(HKEY_LOCAL_MACHINE, keyPath, KEY_READ);
        bool hasDisplayName = key.HasValue("DisplayName");
        if (hasDisplayName)
        {
            displayName = key.GetValue("DisplayName");
            if (displayName.find("Cmajor", 0) != std::string::npos)
            {
                cmajorKeyPath = keyPath;
                return true;
            }
        }
    }
    return false;
}

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    InitDone initDone(instance);
    try
    {
        std::string displayName;
        std::string keyPath;
        if (SearchCmajor(displayName, keyPath))
        {
            throw std::runtime_error("Previous version of Cmajor (" + displayName + ") was found from Windows registry under the following key path:\n\n'" + keyPath + "'.\n\n" + 
                "Please remove the previous version using Control Panel | Add/Remove Programs, and then run the setup program again.");
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    return 0;
}