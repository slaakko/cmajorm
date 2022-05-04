// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SXBS_MESSAGE_INCLUDED
#define SXBS_MESSAGE_INCLUDED
#include <system-x/sxbs/build.hpp>

namespace sxbs {

enum class ReplyKind : int32_t
{
    build, line, exit, stop, error
};

class Reply
{
public:
    Reply(ReplyKind kind_);
    virtual ~Reply();
    virtual std::string ToString() const = 0;
    ReplyKind Kind() const { return kind; }
private:
    ReplyKind kind;
};

enum class RequestKind : int32_t
{
    build, line, exit, stop, error
};

class Request
{
public:
    Request(RequestKind kind_);
    virtual ~Request();
    virtual std::string ToString() const = 0;
    virtual std::unique_ptr<Reply> Execute() = 0;
    RequestKind Kind() const { return kind; }
private:
    RequestKind kind;
};

class BuildRequest : public Request
{
public:
    BuildRequest();
    BuildRequest(const std::string& project_, Config config_);
    std::string ToString() const override;
    std::unique_ptr<Reply> Execute() override;
    void SetServerThread(ServerThread* serverThread_);
private:
    ServerThread* serverThread;
    std::string project;
    Config config;
};

class BuildReply : public Reply
{
public:
    BuildReply(const std::string& error_);
    std::string ToString() const override;
private:
    std::string error;
};

class LineRequest : public Request
{
public:
    LineRequest(const std::string& line_);
    std::string ToString() const override;
    std::unique_ptr<Reply> Execute() override;
private:
    std::string line;
};

class LineReply : public Reply
{
public:
    LineReply();
    std::string ToString() const override;
};

class ExitRequest : public Request
{
public:
    ExitRequest();
    std::string ToString() const override;
    std::unique_ptr<Reply> Execute() override;
};

class ExitReply : public Reply
{
public:
    ExitReply();
    std::string ToString() const override;
};

class StopServerRequest : public Request
{
public:
    StopServerRequest();
    std::string ToString() const override;
    std::unique_ptr<Reply> Execute() override;
};

class StopServerReply : public Reply
{
public:
    StopServerReply();
    std::string ToString() const override;
};

class ErrorRequest : public Request
{
public:
    ErrorRequest(const std::string& error_);
    std::string ToString() const override;
    std::unique_ptr<Reply> Execute() override;
private:
    std::string error;
};

class ErrorReply : public Reply
{
public:
    ErrorReply();
    std::string ToString() const override;
};

std::unique_ptr<Request> ParseRequest(const std::string& requestStr);
std::unique_ptr<Reply> ParseReply(const std::string& replyStr);

} // sxbs

#endif // SXBS_MESSAGE_INCLUDED
