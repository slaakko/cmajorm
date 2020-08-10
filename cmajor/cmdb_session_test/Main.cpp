#include <cmajor/rts/InitDone.hpp>
#include <cmajor/rts/Io.hpp>
#include <cmajor/cmdebug/CmdbSession.hpp>
#include <cmajor/cmdebug/InitDone.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/InitDone.hpp>
#include <stdexcept>
#include <iostream>

#ifndef NDEBUG
#pragma comment(lib, "cmrtsd.lib")
#else
#pragma comment(lib, "cmrts.lib")
#endif
#pragma comment(lib, "pdcurses.lib")
#pragma comment(lib, "ws2_32.lib")

class TestClient : public cmajor::debug::CmdbSessionClient
{
public:
    TestClient(soulng::util::CodeFormatter& formatter_);
    std::string GetTargetInputBytes() override;
    void WriteTargetOuput(int handle, const std::string& s) override;
private:
    soulng::util::CodeFormatter& formatter;
};

TestClient::TestClient(soulng::util::CodeFormatter& formatter_) : formatter(formatter_)
{
}

std::string TestClient::GetTargetInputBytes()
{
    return "foo\n";
}

void TestClient::WriteTargetOuput(int handle, const std::string& s) 
{
    if (handle == 1)
    {
        formatter.Write(s);
    }
    else if (handle == 2)
    {
        std::cerr << s;
    }
}

int main()
{
    static const char buffer[] = { 'f', 'o', 'o', '\n', '\0' };
    std::cout << "foo\n";
    try
    {
        soulng::util::Init();
        cmajor::debug::Init();
        soulng::util::CodeFormatter formatter(std::cout);
        TestClient client(formatter);
        std::string cmdbSessionFilePath;
        std::string exePath = soulng::util::GetFullPath(soulng::util::GetPathToExecutable());
        if (soulng::util::EndsWith(exePath, ".exe"))
        {
            cmdbSessionFilePath = soulng::util::Path::ChangeExtension(exePath, ".cmdbs");
        }
        else
        {
            cmdbSessionFilePath = exePath + ".cmdbs";
        }
        cmajor::debug::StartCmdbSession(cmdbSessionFilePath, formatter, &client, true);
        cmajor::debug::RemoveCmdbSessionFileGuard removeSessionFileGuard(cmdbSessionFilePath);
        RtInit(0, 0, 0, nullptr, nullptr);
        RtWrite(1, reinterpret_cast<const uint8_t*>(buffer), 4);
        cmajor::debug::StopCmdbSession();
        RtDone();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
