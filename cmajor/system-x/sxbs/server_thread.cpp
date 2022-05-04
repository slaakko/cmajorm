// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sxbs/server_thread.hpp>
#include <sxbs/server.hpp>
#include <sxbs/message.hpp>
#include <memory>

namespace sxbs {

ServerThread::ServerThread(soulng::util::TcpSocket&& socket_) : socket(std::move(socket_))
{
}

void ServerThread::Stop()
{
    socket.Close();
}

void ServerThread::Run()
{
    try
    {
        bool exit = false;
        while (!exit)
        {
            std::string requestStr = soulng::util::ReadStr(socket);
            std::unique_ptr<Request> request = ParseRequest(requestStr);
            switch (request->Kind())
            {
                case RequestKind::exit:
                case RequestKind::stop:
                {
                    exit = true;
                    if (request->Kind() == RequestKind::stop)
                    {
                        NotifyStopServer();
                    }
                    break;
                }
                case RequestKind::build:
                {
                    BuildRequest* buildRequest = static_cast<BuildRequest*>(request.get());
                    buildRequest->SetServerThread(this);
                    break;
                }
            }
            std::unique_ptr<Reply> reply = request->Execute();
            std::string replyStr = reply->ToString();
            soulng::util::Write(socket, replyStr);
        }
    }
    catch (...)
    {
        ex = std::current_exception();
    }
}

void ServerThread::SendLogLine(const std::string& logLine)
{
    LineRequest request(logLine);
    soulng::util::Write(socket, request.ToString());
}

void RunServerThread(ServerThread* serverThread)
{
    serverThread->Run();
}

} // sxbs
