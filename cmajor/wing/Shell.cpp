// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Shell.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <stdexcept>
#include <shobjidl.h>
#include <shlobj.h>
#include <Knownfolders.h>
#include <comdef.h>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

void ShellInit()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

void ShellDone()
{
    CoUninitialize();
}

std::string GetCOMErrorMessage(HRESULT hr)
{
    _com_error error(hr);
    const WCHAR* message = error.ErrorMessage();
    std::u16string msg((char16_t*)message);
    return ToUtf8(msg);
}

std::string GetProgramFilesDirectoryPath()
{
    std::string path;
    IKnownFolderManager* manager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&manager));
    if (SUCCEEDED(hr))
    {
        IKnownFolder* folder = nullptr;
        hr = manager->GetFolder(FOLDERID_ProgramFiles, &folder);
        if (SUCCEEDED(hr))
        {
            LPWSTR pathPtr = nullptr;
            hr = folder->GetPath(0, &pathPtr);
            if (SUCCEEDED(hr))
            {
                if (pathPtr)
                {
                    path = GetFullPath(ToUtf8((char16_t*)pathPtr));
                }
                folder->Release();
                manager->Release();
            }
            else
            {
                folder->Release();
                manager->Release();
                throw std::runtime_error("could not get program files directory path: " + GetCOMErrorMessage(hr));
            }
        }
        else
        {
            manager->Release();
            throw std::runtime_error("could not get program files directory path: " + GetCOMErrorMessage(hr));
        }
    }
    else
    {
        throw std::runtime_error("could not get program files directory path: " + GetCOMErrorMessage(hr));
    }
    return path;
}

std::string GetStartMenuProgramsFolderPath()
{
    std::string path;
    IKnownFolderManager* manager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&manager));
    if (SUCCEEDED(hr))
    {
        IKnownFolder* folder = nullptr;
        hr = manager->GetFolder(FOLDERID_CommonPrograms, &folder);
        if (SUCCEEDED(hr))
        {
            LPWSTR pathPtr = nullptr;
            hr = folder->GetPath(0, &pathPtr);
            if (SUCCEEDED(hr))
            {
                if (pathPtr)
                {
                    path = GetFullPath(ToUtf8((char16_t*)pathPtr));
                }
                folder->Release();
                manager->Release();
            }
            else
            {
                folder->Release();
                manager->Release();
                throw std::runtime_error("could not get start menu programs directory path: " + GetCOMErrorMessage(hr));
            }
        }
        else
        {
            manager->Release();
            throw std::runtime_error("could not get start menu programs directory path: " + GetCOMErrorMessage(hr));
        }
    }
    else
    {
        throw std::runtime_error("could not get start menu programs directory path: " + GetCOMErrorMessage(hr));
    }
    return path;
}

std::string GetDesktopFolderPath()
{
    std::string path;
    IKnownFolderManager* manager = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&manager));
    if (SUCCEEDED(hr))
    {
        IKnownFolder* folder = nullptr;
        hr = manager->GetFolder(FOLDERID_Desktop, &folder);
        if (SUCCEEDED(hr))
        {
            LPWSTR pathPtr = nullptr;
            hr = folder->GetPath(0, &pathPtr);
            if (SUCCEEDED(hr))
            {
                if (pathPtr)
                {
                    path = GetFullPath(ToUtf8((char16_t*)pathPtr));
                }
                folder->Release();
                manager->Release();
            }
            else
            {
                folder->Release();
                manager->Release();
                throw std::runtime_error("could not get desktop directory path: " + GetCOMErrorMessage(hr));
            }
        }
        else
        {
            manager->Release();
            throw std::runtime_error("could not get desktop directory path: " + GetCOMErrorMessage(hr));
        }
    }
    else
    {
        throw std::runtime_error("could not get desktop directory path: " + GetCOMErrorMessage(hr));
    }
    return path;
}

void GetShellLinkData(const std::string& linkFilePath, std::string& path, std::string& arguments, std::string& workingDirectory, std::string& description, std::string& iconPath, int& iconIndex)
{
    if (linkFilePath.empty())
    {
        throw std::runtime_error("could not get shell link data: link file path is empty");
    }
    IShellLink* shellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
    const int bufSize = 4096;
    char16_t buffer[bufSize];
    if (SUCCEEDED(hr))
    {
        IPersistFile* file = nullptr;
        hr = shellLink->QueryInterface(IID_IPersistFile, (void**)&file);
        if (SUCCEEDED(hr))
        {
            std::u16string linkP = ToUtf16(MakeNativePath(GetFullPath(linkFilePath)));
            hr = file->Load((LPCOLESTR)linkP.c_str(), STGM_READ);
            if (SUCCEEDED(hr))
            {
                hr = shellLink->GetPath((LPWSTR)&buffer[0], bufSize, nullptr, 0);
                if (SUCCEEDED(hr))
                {
                    path = GetFullPath(ToUtf8(buffer));
                    hr = shellLink->GetArguments((LPWSTR)&buffer[0], bufSize);
                    if (SUCCEEDED(hr))
                    {
                        arguments = ToUtf8(buffer);
                        hr = shellLink->GetWorkingDirectory((LPWSTR)&buffer[0], bufSize);
                        if (SUCCEEDED(hr))
                        {
                            workingDirectory = GetFullPath(ToUtf8(buffer));
                            hr = shellLink->GetDescription((LPWSTR)&buffer[0], bufSize);
                            if (SUCCEEDED(hr))
                            {
                                description = ToUtf8(buffer);
                                int icon = 0;
                                hr = shellLink->GetIconLocation((LPWSTR)&buffer[0], bufSize, &icon);
                                if (SUCCEEDED(hr))
                                {
                                    std::string p = ToUtf8(buffer);
                                    if (!p.empty())
                                    {
                                        iconPath = GetFullPath(p);
                                    }
                                    iconIndex = icon;
                                    file->Release();
                                    shellLink->Release();
                                }
                                else
                                {
                                    file->Release();
                                    shellLink->Release();
                                    throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
                                }
                            }
                            else
                            {
                                file->Release();
                                shellLink->Release();
                                throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
                            }
                        }
                        else
                        {
                            file->Release();
                            shellLink->Release();
                            throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
                        }
                    }
                    else
                    {
                        file->Release();
                        shellLink->Release();
                        throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
                    }
                }
                else
                {
                    file->Release();
                    shellLink->Release();
                    throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
                }
            }
            else
            {
                file->Release();
                shellLink->Release();
                throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
            }
        }
        else
        {
            shellLink->Release();
            throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
        }
    }
    else
    {
        throw std::runtime_error("could not get shell link data: " + GetCOMErrorMessage(hr));
    }
}

void CreateShellLink(const std::string& linkFilePath, const std::string& path, const std::string& arguments, const std::string& workingDirectory, 
    const std::string& description, const std::string& iconPath, int iconIndex)
{
    if (linkFilePath.empty())
    {
        throw std::runtime_error("could not create shell link: link file path is empty");
    }
    if (path.empty())
    {
        throw std::runtime_error("could not create shell link: path is empty");
    }
    IShellLink* shellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shellLink));
    if (SUCCEEDED(hr))
    {
        std::u16string p = ToUtf16(MakeNativePath(GetFullPath(path)));
        hr = shellLink->SetPath((LPCWSTR)p.c_str());
        if (SUCCEEDED(hr))
        {
            if (!arguments.empty())
            {
                std::u16string args = ToUtf16(arguments);
                hr = shellLink->SetArguments((LPCWSTR)args.c_str());
                if (!SUCCEEDED(hr))
                {
                    shellLink->Release();
                    throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
                }
            }
            if (!workingDirectory.empty())
            {
                std::u16string workingDir = ToUtf16(MakeNativePath(GetFullPath(workingDirectory)));
                hr = shellLink->SetWorkingDirectory((LPWSTR)workingDir.c_str());
                if (!SUCCEEDED(hr))
                {
                    shellLink->Release();
                    throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
                }
            }
            if (!description.empty())
            {
                std::u16string desc = ToUtf16(description);
                hr = shellLink->SetDescription((LPCWSTR)desc.c_str());
                if (!SUCCEEDED(hr))
                {
                    shellLink->Release();
                    throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
                }
            }
            if (!iconPath.empty())
            {
                std::u16string iconP = ToUtf16(MakeNativePath(GetFullPath(iconPath)));
                hr = shellLink->SetIconLocation((LPCWSTR)iconP.c_str(), iconIndex);
                if (!SUCCEEDED(hr))
                {
                    shellLink->Release();
                    throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
                }
            }
            IPersistFile* file = nullptr;
            hr = shellLink->QueryInterface(IID_IPersistFile, (void**)&file);
            if (SUCCEEDED(hr))
            {
                std::u16string linkP = ToUtf16(MakeNativePath(GetFullPath(linkFilePath)));
                hr = file->Save((LPCOLESTR)linkP.c_str(), true);
                if (SUCCEEDED(hr))
                {
                    file->Release();
                    shellLink->Release();
                }
                else
                {
                    file->Release();
                    shellLink->Release();
                    throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
                }
            }
            else
            {
                shellLink->Release();
                throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
            }
        }
        else
        {
            shellLink->Release();
            throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
        }
    }
    else
    {
        throw std::runtime_error("could not create shell link: " + GetCOMErrorMessage(hr));
    }
}

} } // cmajor::wing
