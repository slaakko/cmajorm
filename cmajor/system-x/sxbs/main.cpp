// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/sxbs/server.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <system-x/assembler/InitDone.hpp>
#include <system-x/intermediate/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <soulng/util/Util.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    sngcm::ast::Init();
    cmajor::symbols::Init();
    cmsx::machine::Init();
    cmsx::assembler::Init();
    cmsx::intermediate::Init();
    soulng::util::SetLogMode(soulng::util::LogMode::queue);
}

void DoneApplication()
{
    cmsx::intermediate::Done();
    cmsx::assembler::Done();
    cmsx::machine::Done();
    cmajor::symbols::Done();
    sngcm::ast::Done();
    sngxml::xpath::Done();
    soulng::util::Done();
}

struct BackendInit
{
    BackendInit()
    {
        CmmInit(cmajor::mid::BackEndKind::cmsxBackEnd);
    }
    ~BackendInit()
    {
        CmmDone();
    }
};

const char* version = "4.3.0";

void PrintHelp()
{
    // todo
}

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;
using namespace cmajor::build;

int main(int argc, const char** argv)
{
    SetBackEnd(cmajor::symbols::BackEnd::cmsx);
    try
    {
        InitApplication();
        sxbs::RunServer();
    }
    catch (const std::exception& ex)
    {
        if (!GetGlobalFlag(GlobalFlags::quiet))
        {
            std::cout << "sx.bs.error" << std::endl;
            std::cout << ex.what() << std::endl;
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    DoneApplication();
    return 0;
}
