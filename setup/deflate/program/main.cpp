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
using namespace wing;
using namespace wingstall::winggui;
using namespace wingstall::wingpackage;

void InitApplication(HINSTANCE instance)
{
    soulng::util::Init();
    sngxml::xpath::Init();
    wing::Init(instance);
}

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    try
    {
        InitApplication(instance);
        std::string currentExecutableName = Path::GetFileName(GetFullPath(GetPathToExecutable()));
        BinaryResourcePtr unicodeDBResource(currentExecutableName, setup::UnicodeDBResourceName());
        CharacterTable::Instance().SetDeflateData(unicodeDBResource.Data(), unicodeDBResource.Size(), setup::UncompressedUnicodeDBSize());
        BinaryResourcePtr packageResource(currentExecutableName, setup::PackageResourceName());
        SetInfoItem(InfoItemKind::appName, new StringItem(setup::AppName()));
        SetInfoItem(InfoItemKind::appVersion, new StringItem(setup::AppVersion()));
        SetInfoItem(InfoItemKind::installDirName, new StringItem(setup::InstallDirName()));
        SetInfoItem(InfoItemKind::defaultContainingDirPath, new StringItem(setup::DefaultContainingDirPath()));
        SetInfoItem(InfoItemKind::compression, new IntegerItem(static_cast<int64_t>(setup::Compression())));
        SetInfoItem(InfoItemKind::dataSource, new IntegerItem(static_cast<int64_t>(DataSource::memory)));
        SetInfoItem(InfoItemKind::packageDataAddress, new IntegerItem(reinterpret_cast<int64_t>(packageResource.Data())));
        SetInfoItem(InfoItemKind::compressedPackageSize, new IntegerItem(packageResource.Size()));
        SetInfoItem(InfoItemKind::uncompressedPackageSize, new IntegerItem(setup::UncompressedPackageSize()));
        Icon& setupIcon = Application::GetResourceManager().GetIcon(setup::SetupIconResourceName());
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

