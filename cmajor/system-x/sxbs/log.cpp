// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sxbs/log.hpp>
#include <sxbs/server_thread.hpp>
#include <soulng/util/Log.hpp>

namespace sxbs {

void SendLog(ServerThread* serverThread, std::exception_ptr& ex)
{
    try
    {
        bool endOfLog = false;
        bool timeOut = false;
        std::string line = soulng::util::FetchLogMessage(endOfLog, 0, timeOut);
        while (!endOfLog)
        {
            serverThread->SendLogLine(line);
            line = soulng::util::FetchLogMessage(endOfLog, 0, timeOut);
        }
        ex = std::exception_ptr();
    }
    catch (...)
    {
        ex = std::current_exception();
    }
}

} // sxbs
