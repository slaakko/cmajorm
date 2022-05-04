// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SXBS_LOG_INCLUDED
#define SXBS_LOG_INCLUDED
#include <system-x/sxbs/server_thread.hpp>
#include <exception>

namespace sxbs {

void SendLog(ServerThread* serverThread, std::exception_ptr& ex);

} // sxbs

#endif // SXBS_LOG_INCLUDED
