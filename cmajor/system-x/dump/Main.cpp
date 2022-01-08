// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/TextFormat.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <system-x/assembler/InitDone.hpp>
#include <system-x/kernel/InitDone.hpp>
#include <system-x/kernel/Trap.hpp>
#include <soulng/util/Path.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace soulng::util;

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    cmsx::machine::Init();
    cmsx::assembler::Init();
    cmsx::kernel::Init();
}

void DoneApplication()
{
    cmsx::kernel::Done();
    cmsx::assembler::Done();
    cmsx::machine::Done();
    sngxml::xpath::Done();
    soulng::util::Done();
}

void TrapAdder(cmsx::object::SymbolTable& symbolTable)
{
    cmsx::kernel::AddTrapsToSymbolTable(symbolTable);
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        cmsx::machine::Machine machine;
        bool verbose = false;
        std::vector<std::string> fileNames;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg.starts_with("--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else
                {
                    throw std::runtime_error("unknown argument '" + arg + "'");
                }
            }
            else if (arg.starts_with("-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'v':
                        {
                            verbose = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown argument '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                fileNames.push_back(GetFullPath(arg));
            }
        }
        cmsx::object::SetTrapAdderFunc(TrapAdder);
        for (const auto& fileName : fileNames)
        {
            if (verbose)
            {
                std::cout << "> " << fileName << std::endl;
            }
            std::string textFileName;
            cmsx::object::WriteBinaryFileAsText(fileName, machine, textFileName);
            if (verbose)
            {
                std::cout << "==> " << textFileName << std::endl;
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}