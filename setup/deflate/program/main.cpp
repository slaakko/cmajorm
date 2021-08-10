#include <winggui/install_window.hpp>
#include "data.hpp"
#include <stdexcept>
#include <wing/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>

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
        SetInfoItem(InfoItemKind::appName, new StringItem("Cmajor"));
        SetInfoItem(InfoItemKind::appVersion, new StringItem("4.1.0"));
        SetInfoItem(InfoItemKind::installDirName, new StringItem("cmajor"));
        SetInfoItem(InfoItemKind::defaultContainingDirPath, new StringItem("C:/"));
        SetInfoItem(InfoItemKind::compression, new IntegerItem(static_cast<int64_t>(Compression::deflate)));
        SetInfoItem(InfoItemKind::dataSource, new IntegerItem(static_cast<int64_t>(DataSource::memory)));
        SetInfoItem(InfoItemKind::compressedPackageSize, new IntegerItem(197218280));
        SetInfoItem(InfoItemKind::uncompressedPackageSize, new IntegerItem(1368366183));
        Package package;
        InstallWindow installWindow;
        installWindow.SetPackage(&package);
        Application::Run(installWindow);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    return 0;
}

