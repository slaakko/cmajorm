#include <winggui/install_window.hpp>
#include "data.hpp"
#include <stdexcept>
#include <wing/BinaryResourcePtr.hpp>
#include <wing/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/Unicode.hpp>

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::wing;
using namespace wingstall::winggui;
using namespace wingstall::wingpackage;

struct InitDone
{
    InitDone(HINSTANCE instance)
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::wing::Init(instance);
    }
    ~InitDone()
    {
        cmajor::wing::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    InitDone initDone(instance);
    try
    {
        std::string currentExecutableName = Path::GetFileName(GetFullPath(GetPathToExecutable()));
        BinaryResourcePtr unicodeDBResource(currentExecutableName, UnicodeDBResourceName());
        CharacterTable::Instance().SetDeflateData(unicodeDBResource.Data(), unicodeDBResource.Size(), UncompressedUnicodeDBSize());
        BinaryResourcePtr packageResource(currentExecutableName, PackageResourceName());
        SetInfoItem(InfoItemKind::appName, new StringItem("Cmajor"));
        SetInfoItem(InfoItemKind::appVersion, new StringItem("4.1.0"));
        SetInfoItem(InfoItemKind::installDirName, new StringItem("cmajor"));
        SetInfoItem(InfoItemKind::defaultContainingDirPath, new StringItem("C:/"));
        SetInfoItem(InfoItemKind::compression, new IntegerItem(static_cast<int64_t>(Compression::none)));
        SetInfoItem(InfoItemKind::dataSource, new IntegerItem(static_cast<int64_t>(DataSource::memory)));
        SetInfoItem(InfoItemKind::packageDataAddress, new IntegerItem(reinterpret_cast<int64_t>(packageResource.Data())));
        SetInfoItem(InfoItemKind::compressedPackageSize, new IntegerItem(packageResource.Size()));
        SetInfoItem(InfoItemKind::uncompressedPackageSize, new IntegerItem(UncompressedPackageSize()));
        Icon& setupIcon = Application::GetResourceManager().GetIcon(SetupIconResourceName());
        Package package;
        InstallWindow installWindow;
        installWindow.SetPackage(&package);
        installWindow.SetIcon(setupIcon);
        installWindow.SetSmallIcon(setupIcon);
        Application::Run(installWindow);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    return 0;
}

