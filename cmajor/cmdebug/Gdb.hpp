// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_INFO_GDB_INCLUDED
#define CMAJOR_DEBUG_INFO_GDB_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace cmajor { namespace debug {

using namespace soulng::util;

class DEBUG_API GdbCommand
{
public:
    enum class Kind
    {
        exit, breakInsert, breakDelete, execContinue, execFinish, execNext, execStep, execUntil, execRun, stackInfoDepth, stackListFrames
    };
    GdbCommand(Kind kind_, const std::string& str_);
    GdbCommand(const GdbCommand&) = delete;
    GdbCommand(GdbCommand&&) = delete;
    GdbCommand& operator=(const GdbCommand&) = delete;
    GdbCommand& operator=(GdbCommand&&) = delete;
    virtual ~GdbCommand();
    Kind GetKind() const { return kind; }
    const std::string& Str() const { return str; }
private:
    Kind kind;
    std::string str;
};

class DEBUG_API GdbBreakInsertCommand : public GdbCommand
{
public:
    GdbBreakInsertCommand(const std::string& location);
};

class DEBUG_API GdbBreakDeleteCommand : public GdbCommand
{
public:
    GdbBreakDeleteCommand(const std::string& breakpoints);
};

class DEBUG_API GdbExecContinueCommand : public GdbCommand
{
public:
    GdbExecContinueCommand();
};

class DEBUG_API GdbExecFinishCommand : public GdbCommand
{
public:
    GdbExecFinishCommand();
};

class DEBUG_API GdbExecNextCommand : public GdbCommand
{
public:
    GdbExecNextCommand();
};

class DEBUG_API GdbExecStepCommand : public GdbCommand
{
public:
    GdbExecStepCommand();
};

class DEBUG_API GdbExecUntilCommand : public GdbCommand
{
public:
    GdbExecUntilCommand(const std::string& location);
};

class DEBUG_API GdbExecRunCommand : public GdbCommand
{
public:
    GdbExecRunCommand();
};

class DEBUG_API GdbExitCommand : public GdbCommand
{
public:
    GdbExitCommand();
};

class DEBUG_API GdbStackInfoDepthCommand : public GdbCommand
{
public:
    GdbStackInfoDepthCommand();
};

class DEBUG_API GdbStackListFramesCommand : public GdbCommand
{
public:
    GdbStackListFramesCommand(int lowFrame, int highFrame);
};

class DEBUG_API GdbValue
{
public:
    enum class Kind
    {
        result, string, tuple, list
    };
    GdbValue(Kind kind_);
    GdbValue(const GdbValue&) = delete;
    GdbValue(GdbValue&&) = delete;
    GdbValue& operator=(const GdbValue&) = delete;
    GdbValue& operator=(GdbValue&&) = delete;
    virtual ~GdbValue();
    Kind GetKind() const { return kind; }
    virtual void Print(CodeFormatter& formatter) = 0;
    virtual std::string ToString() const = 0;
private:
    Kind kind;
};

class DEBUG_API GdbStringValue : public GdbValue
{
public:
    GdbStringValue(const std::string& value_);
    const std::string& Value() const { return value; }
    void Print(CodeFormatter& formatter) override;
    std::string ToString() const override;
private:
    std::string value;
};

class GdbResult;

class DEBUG_API GdbTupleValue : public GdbValue
{
public:
    GdbTupleValue();
    GdbTupleValue(const GdbTupleValue&) = delete;
    GdbTupleValue(GdbTupleValue&&) = delete;
    GdbTupleValue& operator=(const GdbTupleValue&) = delete;
    GdbTupleValue& operator=(GdbTupleValue&&) = delete;
    void AddResult(GdbResult* result);
    const std::vector<std::unique_ptr<GdbResult>>& Results() const { return results; }
    void Print(CodeFormatter& formatter) override;
    std::string ToString() const override;
    GdbValue* GetField(const std::string& fieldName) const;
private:
    std::vector<std::unique_ptr<GdbResult>> results;
    std::unordered_map<std::string, GdbValue*> fieldMap;
};

class DEBUG_API GdbListValue : public GdbValue
{
public:
    GdbListValue();
    GdbListValue(const GdbListValue&) = delete;
    GdbListValue(GdbListValue&&) = delete;
    GdbListValue& operator=(const GdbListValue&) = delete;
    GdbListValue& operator=(GdbListValue&&) = delete;
    void AddValue(GdbValue* value);
    const std::vector<std::unique_ptr<GdbValue>>& Values() const { return values; }
    void Print(CodeFormatter& formatter) override;
    std::string ToString() const override;
    int Count() const { return values.size(); }
    GdbValue* GetValue(int index) const;
private:
    std::vector<std::unique_ptr<GdbValue>> values;
};

class DEBUG_API GdbResult : public GdbValue
{
public:
    GdbResult(const std::string& name_, GdbValue* value_);
    GdbResult(const GdbResult&) = delete;
    GdbResult(GdbResult&&) = delete;
    GdbResult& operator=(const GdbResult&) = delete;
    GdbResult& operator=(GdbResult&&) = delete;
    void Print(CodeFormatter& formatter);
    const std::string& Name() const { return name; }
    GdbValue* Value() const { return value.get(); }
    std::string ToString() const override;
private:
    std::string name;
    std::unique_ptr<GdbValue> value;
};

class DEBUG_API GdbResults
{
public:
    GdbResults();
    GdbResults(const GdbResults&) = delete;
    GdbResults(GdbResults&&) = delete;
    GdbResults& operator=(const GdbResults&) = delete;
    GdbResults& operator=(GdbResults&&) = delete;
    void Add(GdbResult* result);
    void Print(CodeFormatter& formatter);
    int Count() const { return results.size(); }
    GdbResult* operator[](int index) const { return results[index].get(); }
    GdbValue* GetField(const std::string& fieldName) const;
private:
    std::vector<std::unique_ptr<GdbResult>> results;
    std::unordered_map<std::string, GdbValue*> fieldMap;
};

class DEBUG_API GdbReplyRecord
{
public:
    enum class Kind : uint8_t
    {
        result, execRunning, execStopped, notification, consoleOutput, targetOutput, logOutput, prompt, parsingError
    };
    const char* KindStr() const;
    GdbReplyRecord(Kind kind_, GdbResults* results_);
    GdbReplyRecord(const GdbReplyRecord&) = delete;
    GdbReplyRecord(GdbReplyRecord&&) = delete;
    GdbReplyRecord& operator=(const GdbReplyRecord&) = delete;
    GdbReplyRecord& operator=(GdbReplyRecord&&) = delete;
    virtual ~GdbReplyRecord();
    Kind GetKind() const { return kind; }
    virtual void Print(CodeFormatter& formatter);
    virtual bool Stopped() const { return false; }
    virtual bool CommandSucceeded() const { return kind != Kind::parsingError; }
    GdbResults* Results() const { return results.get(); }
private:
    Kind kind;
    std::unique_ptr<GdbResults> results;
};

class DEBUG_API GdbResultRecord : public GdbReplyRecord
{
public:
    enum class Class : uint8_t
    {
        done, running, connected, error, exit
    };
    GdbResultRecord(Class cls, GdbResults* results);
    const char* ClassStr() const;
    void Print(CodeFormatter& formatter) override;
    bool CommandSucceeded() const override { return cls != Class::error; }
    Class GetClass() const { return cls; }
private:
    Class cls;
};

class DEBUG_API GdbDoneRecord : public GdbResultRecord
{
public:
    GdbDoneRecord(GdbResults* results);
    bool Stopped() const override { return true; }
};

class DEBUG_API GdbRunningRecord : public GdbResultRecord
{
public:
    GdbRunningRecord(GdbResults* results);
};

class DEBUG_API GdbConnectedRecord : public GdbResultRecord
{
public:
    GdbConnectedRecord(GdbResults* results);
};

class DEBUG_API GdbErrorRecord : public GdbResultRecord
{
public:
    GdbErrorRecord(GdbResults* results);
    bool Stopped() const override { return true; }
    std::string Msg() const;
    std::string Code() const;
};

class DEBUG_API GdbExitRecord : public GdbResultRecord
{
public:
    GdbExitRecord(GdbResults* results);
    bool Stopped() const override { return true; }
};

class DEBUG_API GdbAsyncRecord : public GdbReplyRecord
{
public:
    GdbAsyncRecord(Kind kind, GdbResults* results);
};

class DEBUG_API GdbExecRecord : public GdbAsyncRecord
{
public:
    GdbExecRecord(Kind kind, GdbResults* results);
};

class DEBUG_API GdbExecRunningRecord : public GdbExecRecord
{
public:
    GdbExecRunningRecord(GdbResults* results);
};

class DEBUG_API GdbExecStoppedRecord : public GdbExecRecord
{
public:
    GdbExecStoppedRecord(GdbResults* results);
    bool Stopped() const override { return true; }
};

class DEBUG_API GdbNotifyAsyncRecord : public GdbAsyncRecord
{
public:
    GdbNotifyAsyncRecord(const std::string& notification_, GdbResults* results);
    void Print(CodeFormatter& formatter);
private:
    std::string notification;
};

class DEBUG_API GdbStreamRecord : public GdbReplyRecord
{
public:
    GdbStreamRecord(Kind kind, const std::string& text_);
    const std::string& Text() const { return text; }
    void Print(CodeFormatter& formatter) override;
private:
    std::string text;
};

class DEBUG_API GdbConsoleOutputRecord : public GdbStreamRecord
{
public:
    GdbConsoleOutputRecord(const std::string& text);
};

class DEBUG_API GdbTargetOutputRecord : public GdbStreamRecord
{
public:
    GdbTargetOutputRecord(const std::string& text);
};

class DEBUG_API GdbLogOutputRecord : public GdbStreamRecord
{
public:
    GdbLogOutputRecord(const std::string& text);
};

class DEBUG_API GdbPrompt : public GdbReplyRecord
{
public:
    GdbPrompt();
};

class DEBUG_API GdbParsingError : public GdbReplyRecord
{
public:
    GdbParsingError(const std::string& parsingError_);
    const std::string& ParsingError() const { return parsingError; }
    void Print(CodeFormatter& formatter) override;
private:
    std::string parsingError;
};

class DEBUG_API GdbReply
{
public:
    GdbReply();
    GdbReply(const GdbReply&) = delete;
    GdbReply(GdbReply&&) = delete;
    GdbReply& operator=(const GdbReply&) = delete;
    GdbReply& operator=(GdbReply&&) = delete;
    virtual ~GdbReply();
    void SetTextLines(const std::vector<std::string>& textLines_);
    const std::vector<std::string>& TextLines() const { return textLines; }
    void AddReplyRecord(std::unique_ptr<GdbReplyRecord>&& replyRecord);
    const std::vector<std::unique_ptr<GdbReplyRecord>>& ReplyRecords() const { return replyRecords; }
    void Print(CodeFormatter& formatter);
    GdbResultRecord* GetResultRecord() const { return resultRecord; }
    GdbReplyRecord* GetStoppedRecord() const { return stoppedRecord; }
private:
    std::vector<std::string> textLines;
    std::vector<std::unique_ptr<GdbReplyRecord>> replyRecords;
    GdbResultRecord* resultRecord;
    GdbReplyRecord* stoppedRecord;
};

DEBUG_API void SetDebugFlag();
DEBUG_API void StartGDB(const std::string& executable, const std::vector<std::string>& args);
DEBUG_API GdbReply* GetGDBStartReply();
DEBUG_API std::unique_ptr<GdbReply> ExecuteGDBCommand(const GdbCommand& command);
DEBUG_API std::unique_ptr<GdbReply> ReadGDBReply();
DEBUG_API void StopGDB();
DEBUG_API void InitGDB();
DEBUG_API void DoneGDB();

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_INFO_GDB_INCLUDED
