// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sxbs/message.hpp>
#include <sxbs/build.hpp>
#include <sxbs/tokenizer.hpp>
#include <stdexcept>

namespace sxbs {

Reply::Reply(ReplyKind kind_) : kind(kind_)
{
}

Reply::~Reply()
{
}

Request::Request(RequestKind kind_) : kind(kind_)
{
}

Request::~Request()
{
}

BuildRequest::BuildRequest() : Request(RequestKind::build), serverThread(), project(), config()
{
}

BuildRequest::BuildRequest(const std::string& project_, Config config_) :
    Request(RequestKind::build), serverThread(), project(project_), config(config_)
{
}

std::string BuildRequest::ToString() const
{
    return "build request " + project;
}

std::unique_ptr<Reply> BuildRequest::Execute()
{
    std::string error;
    try
    {
        if (!serverThread)
        {
            throw std::runtime_error("server thread not set");
        }
        Build(serverThread, project, config);
    }
    catch (const std::exception& ex)
    {
        error = ex.what();
    }
    catch (...)
    {
        error = "build failed: unknown reason";
    }
    return std::unique_ptr<Reply>(new BuildReply(error));
}

void BuildRequest::SetServerThread(ServerThread* serverThread_)
{
    serverThread = serverThread_;
}

BuildReply::BuildReply(const std::string& error_) : Reply(ReplyKind::build), error(error_)
{
}

std::string BuildReply::ToString() const
{
    return "build reply " + error;
}

LineRequest::LineRequest(const std::string& line_) : Request(RequestKind::line), line(line_)
{
}

std::string LineRequest::ToString() const
{
    return "line request " + line;
}

std::unique_ptr<Reply> LineRequest::Execute()
{
    return std::unique_ptr<Reply>(new LineReply());
}

LineReply::LineReply() : Reply(ReplyKind::line)
{
}

std::string LineReply::ToString() const
{
    return "line reply";
}

ExitRequest::ExitRequest() : Request(RequestKind::exit)
{
}

std::string ExitRequest::ToString() const
{
    return "exit request";
}

std::unique_ptr<Reply> ExitRequest::Execute()
{
    return std::unique_ptr<Reply>(new ExitReply());
}

ExitReply::ExitReply() : Reply(ReplyKind::exit)
{
}

std::string ExitReply::ToString() const
{
    return "exit reply";
}

StopServerRequest::StopServerRequest() : Request(RequestKind::stop)
{
}

std::string StopServerRequest::ToString() const
{
    return "stop server request";
}

StopServerReply::StopServerReply() : Reply(ReplyKind::stop)
{
}

std::unique_ptr<Reply> StopServerRequest::Execute()
{
    return std::unique_ptr<Reply>(new StopServerReply());
}

std::string StopServerReply::ToString() const
{
    return "stop server reply";
}

ErrorRequest::ErrorRequest(const std::string& error_) : Request(RequestKind::error), error(error_)
{
}

std::string ErrorRequest::ToString() const
{
    return "error request " + error;
}

std::unique_ptr<Reply> ErrorRequest::Execute()
{
    return std::unique_ptr<Reply>(new ErrorReply());
}

ErrorReply::ErrorReply() : Reply(ReplyKind::error)
{
}

std::string ErrorReply::ToString() const
{
    return "error reply";
}

bool ParseBuildRequest(Tokenizer& tokenizer, std::unique_ptr<Request>& request)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == BUILD)
    {
        ++tokenizer;
        Config config = Config::none;
        if (*tokenizer == DEBUG)
        {
            config = config | Config::debug;
            ++tokenizer;
        }
        if (*tokenizer == RELEASE)
        {
            config = config | Config::release;
            ++tokenizer;
        }
        if (config == Config::none)
        {
            config = Config::both;
        }
        if (*tokenizer == REQUEST)
        {
            ++tokenizer;
            std::string project;
            bool first = true;
            while (*tokenizer != END)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    project.append(1, ' ');
                }
                project.append(tokenizer.GetWord(tokenizer.Pos()));
                ++tokenizer;
            }
            if (!project.empty())
            {
                request.reset(new BuildRequest(project, config));
                return true;
            }
        }
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseExitRequest(Tokenizer& tokenizer, std::unique_ptr<Request>& request)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == EXIT)
    {
        ++tokenizer;
        if (*tokenizer == REQUEST)
        {
            ++tokenizer;
            request.reset(new ExitRequest());
            return true;
        }
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseStopServerRequest(Tokenizer& tokenizer, std::unique_ptr<Request>& request)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == STOP)
    {
        ++tokenizer;
        if (*tokenizer == SERVER)
        {
            ++tokenizer;
            request.reset(new StopServerRequest());
            return true;
        }
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseLineRequest(Tokenizer& tokenizer, std::unique_ptr<Request>& request)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == LINE)
    {
        ++tokenizer;
        if (*tokenizer == REQUEST)
        {
            ++tokenizer;
            std::string line;
            bool first = true;
            while (*tokenizer != END)
            {
                if (!first)
                {
                    first = false;
                }
                else
                {
                    line.append(" ");
                }
                std::string word = tokenizer.GetWord(tokenizer.Pos());
                line.append(word);
                request.reset(new LineRequest(line));
                ++tokenizer;
            }
        }
        return true;
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseErrorRequest(Tokenizer& tokenizer, std::unique_ptr<Request>& request)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == ERROR)
    {
        ++tokenizer;
        if (*tokenizer == REQUEST)
        {
            ++tokenizer;
            std::string error;
            bool first = true;
            while (*tokenizer != END)
            {
                if (!first)
                {
                    first = false;
                }
                else
                {
                    error.append(" ");
                }
                std::string word = tokenizer.GetWord(tokenizer.Pos());
                error.append(word);
                request.reset(new ErrorRequest(error));
                ++tokenizer;
            }
        }
    }
    tokenizer.SetPos(save);
    return false;
}

std::unique_ptr<Request> ParseRequest(const std::string& requestStr)
{
    Tokenizer tokenizer(requestStr);
    std::unique_ptr<Request> request;
    switch (*tokenizer)
    {
        case BUILD:
        {
            if (ParseBuildRequest(tokenizer, request))
            {
                return request;
            }
            break;
        }
        case LINE:
        {
            if (ParseLineRequest(tokenizer, request))
            {
                return request;
            }
            break;
        }
        case EXIT:
        {
            if (ParseExitRequest(tokenizer, request))
            {
                return request;
            }
            break;
        }
        case STOP:
        {
            if (ParseStopServerRequest(tokenizer, request))
            {
                return request;
            }
            break;
        }
        case ERROR:
        {
            if (ParseErrorRequest(tokenizer, request))
            {
                return request;
            }
            break;
        }
    }
    request.reset(new ErrorRequest("request parsing error:\n" + tokenizer.ErrorLines()));
    return request;
}

bool ParseBuildReply(Tokenizer& tokenizer, std::unique_ptr<Reply>& reply)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == BUILD)
    {
        ++tokenizer;
        if (*tokenizer == REPLY)
        {
            ++tokenizer;
            std::string error;
            bool first = true;
            while (*tokenizer != END)
            {
                if (!first)
                {
                    first = false;
                }
                else
                {
                    error.append(" ");
                }
                std::string word = tokenizer.GetWord(tokenizer.Pos());
                error.append(word);
                ++tokenizer;
            }
            reply.reset(new BuildReply(error));
            return true;
        }
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseExitReply(Tokenizer& tokenizer, std::unique_ptr<Reply>& reply)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == EXIT)
    {
        ++tokenizer;
        if (*tokenizer == REPLY)
        {
            ++tokenizer;
            reply.reset(new ExitReply());
            return true;
        }
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseStopServerReply(Tokenizer& tokenizer, std::unique_ptr<Reply>& reply)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == STOP)
    {
        ++tokenizer;
        if (*tokenizer == SERVER)
        {
            ++tokenizer;
            if (*tokenizer == REPLY)
            {
                ++tokenizer;
                reply.reset(new StopServerReply());
                return true;
            }
        }
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseLineReply(Tokenizer& tokenizer, std::unique_ptr<Reply>& reply)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == LINE)
    {
        ++tokenizer;
        if (*tokenizer == REPLY)
        {
            reply.reset(new LineReply());
            return true;
        }
    }
    tokenizer.SetPos(save);
    return false;
}

bool ParseErrorReply(Tokenizer& tokenizer, std::unique_ptr<Reply>& reply)
{
    int32_t save = tokenizer.Pos();
    if (*tokenizer == ERROR)
    {
        ++tokenizer;
        if (*tokenizer == REPLY)
        {
            reply.reset(new ErrorReply());
            return true;
        }
    }
    tokenizer.SetPos(save);
    return false;
}

std::unique_ptr<Reply> ParseReply(const std::string& replyStr)
{
    Tokenizer tokenizer(replyStr);
    std::unique_ptr<Reply> reply;
    switch (*tokenizer)
    {
        case BUILD:
        {
            if (ParseBuildReply(tokenizer, reply))
            {
                return reply;
            }
            break;
        }
        case LINE:
        {
            if (ParseLineReply(tokenizer, reply))
            {
                return reply;
            }
            break;
        }
        case EXIT:
        {
            if (ParseExitReply(tokenizer, reply))
            {
                return reply;
            }
            break;
        }
        case STOP:
        {
            if (ParseStopServerReply(tokenizer, reply))
            {
                return reply;
            }
            break;
        }
        case ERROR:
        {
            if (ParseErrorReply(tokenizer, reply))
            {
                return reply;
            }
            break;
        }
    }
    return reply;
}

} // sxbs
