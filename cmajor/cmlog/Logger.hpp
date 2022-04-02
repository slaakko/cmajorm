// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Socket.hpp>

using namespace soulng::util;

class Logger
{
public:
    Logger(TcpSocket&& socket_);
    void Run();
    void Stop();
private:
    TcpSocket socket;
    bool exiting;
};

void RunLogger(Logger* logger);