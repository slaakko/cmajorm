// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Trace.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <thread>
#include <vector>
#include <stdexcept>
#include <mutex>

namespace soulng { namespace util {

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return std::string(e);
}

std::string TraceDir()
{
    std::string root = CmajorRootDir();
    std::string traceDir = GetFullPath(Path::Combine(root, "trace"));
    boost::filesystem::create_directories(traceDir);
    return traceDir;
}

std::string ConfigDir()
{
    std::string root = CmajorRootDir();
    std::string configDir = GetFullPath(Path::Combine(root, "config"));
    boost::filesystem::create_directories(configDir);
    return configDir;
}

std::string TraceFilePath()
{
    std::string traceFilePath = GetFullPath(Path::Combine(TraceDir(), "trace.bin"));
    return traceFilePath;
}

std::string WinMsgFilePath()
{
    std::string winMsgFilePath = GetFullPath(Path::Combine(ConfigDir(), "winmsg.txt"));
    return winMsgFilePath;
}

class TraceTable
{
public:
    static void Init();
    static void Done();
    static TraceTable& Instance() { return *instance; }
    int32_t GetTraceFunctionId(const std::string& functionFullName);
    const std::string& GetTraceFunctionName(int32_t traceFunctionId) const;
    const std::string& GetTraceMessageName(int32_t traceMessageId) const;
    void Read();
    void Write();
private:
    std::mutex mtx;
    static std::unique_ptr<TraceTable> instance;
    std::vector<std::string> traceFunctions;
    std::map<std::string, int32_t> traceFunctionMap;
    std::map<int32_t, std::string> traceMessageMap;
    std::string unknownMessage;
    TraceTable();
};

std::string TraceTablePath()
{
    return GetFullPath(Path::Combine(TraceDir(), "trace.tab"));
}

std::unique_ptr<TraceTable> TraceTable::instance;

void TraceTable::Init()
{
    instance.reset(new TraceTable());
}

void TraceTable::Done()
{
    instance.reset();
}

TraceTable::TraceTable() : unknownMessage("<UNKNOWN_WINDOWS_MESSSAGE>")
{
}

int32_t TraceTable::GetTraceFunctionId(const std::string& functionFullName)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = traceFunctionMap.find(functionFullName);
    if (it != traceFunctionMap.cend())
    {
        return it->second;
    }
    int32_t traceFunctionId = traceFunctions.size();
    traceFunctions.push_back(functionFullName);
    traceFunctionMap[functionFullName] = traceFunctionId;
    return traceFunctionId;
}

const std::string& TraceTable::GetTraceFunctionName(int32_t traceFunctionId) const
{
    Assert(traceFunctionId >= 0 && traceFunctionId < traceFunctions.size(), "invalid trace function id");
    return traceFunctions[traceFunctionId];
}

const std::string& TraceTable::GetTraceMessageName(int32_t traceMessageId) const
{
    auto it = traceMessageMap.find(traceMessageId);
    if (it != traceMessageMap.cend())
    {
        return it->second;
    }
    else
    {
        return unknownMessage;
    }
}

void TraceTable::Read()
{
    std::string traceTablePath = TraceTablePath();
    std::ifstream traceTableFile(traceTablePath);
    std::string line;
    while (std::getline(traceTableFile, line))
    {
        std::string::size_type colonPos = line.find(':');
        std::string indexStr = line.substr(0, colonPos);
        int32_t index = boost::lexical_cast<int32_t>(indexStr);
        std::string functionName = line.substr(colonPos + 1);
        Assert(index == traceFunctions.size(), "index invalid");
        traceFunctions.push_back(functionName);
        traceFunctionMap[functionName] = index;
    }
    std::string winMsgFilePath = WinMsgFilePath();
    std::ifstream winMsgFile(winMsgFilePath);
    std::string msgLine;
    while (std::getline(winMsgFile, msgLine))
    {
        if (msgLine.empty()) continue;
        std::string::size_type colonPos = msgLine.find(':');
        if (colonPos == std::string::npos) continue;
        std::string indexStr = msgLine.substr(0, colonPos);
        int32_t index = boost::lexical_cast<int32_t>(indexStr);
        std::string messageName = msgLine.substr(colonPos + 1);
        if (traceMessageMap.find(index) == traceMessageMap.cend())
        {
            traceMessageMap[index] = messageName;
        }
    }
}

void TraceTable::Write()
{
    std::string traceTablePath = TraceTablePath();
    std::ofstream traceTableFile(traceTablePath);
    CodeFormatter formatter(traceTableFile);
    int32_t n = traceFunctions.size();
    for (int32_t i = 0; i < n; ++i)
    {
        auto it = traceFunctionMap.find(traceFunctions[i]);
        int32_t index = -1;
        if (it != traceFunctionMap.cend())
        {
            index = it->second;
        }
        Assert(index == i, "index invalid");
        formatter << i << ":" << traceFunctions[i] << std::endl;
    }
}

void ReadTraceTable()
{
    TraceTable::Instance().Read();
}

void WriteTraceTable()
{
    TraceTable::Instance().Write();
}

int32_t GetTraceFunctionId(const std::string& functionFullName)
{
    return TraceTable::Instance().GetTraceFunctionId(functionFullName);
}

struct TraceEntry
{
    TraceEntry(EntryKind kind_, char threadId_, int32_t id_, int64_t nanosecs_);
    EntryKind kind;
    char threadId;
    int32_t id;
    int64_t nanosecs;
};

TraceEntry::TraceEntry(EntryKind kind_, char threadId_, int32_t id_, int64_t nanosecs_) : kind(kind_), threadId(threadId_), id(id_), nanosecs(nanosecs_)
{
}

class Trace
{
public:
    static void Init();
    static void Done();
    static Trace& Instance() { return *instance; }
    void AddEntry(const TraceEntry& entry);
    void Write();
private:
    static std::unique_ptr<Trace> instance;
    std::vector<TraceEntry> entries;
    std::mutex mtx;
};

std::unique_ptr<Trace> Trace::instance;

void Trace::Init()
{
    instance.reset(new Trace());
}

void Trace::Done()
{
    instance.reset();
}

void Trace::AddEntry(const TraceEntry& entry)
{
    std::lock_guard<std::mutex> lock(mtx);
    entries.push_back(entry);
}

void Trace::Write()
{
    BinaryWriter writer(TraceFilePath());
    int32_t n = entries.size();
    writer.Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        const TraceEntry& entry = entries[i];
        writer.Write(static_cast<int8_t>(entry.kind));
        writer.Write(entry.threadId);
        writer.Write(entry.id);
        writer.Write(entry.nanosecs);
    }
}

std::vector<TraceEntry> ReadTrace(const std::string& traceFilePath)
{
    std::vector<TraceEntry> trace;
    BinaryReader reader(traceFilePath);
    int32_t n = reader.ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        EntryKind kind = static_cast<EntryKind>(reader.ReadSByte());
        char threadId = reader.ReadChar();
        int32_t id = reader.ReadInt();
        int64_t nanosecs = reader.ReadLong();
        TraceEntry entry(kind, threadId, id, nanosecs);
        trace.push_back(std::move(entry));
    }
    return trace;
}

struct Thread
{
    Thread(char id_) : id(id_), indent(0) {}
    char id;
    int indent;
    std::vector<TraceEntry*> startEntries;
};

class ThreadMap
{
public:
    Thread* GetThread(char threadId);
private:
    std::vector<std::unique_ptr<Thread>> threads;
    std::map<char, Thread*> threadMap;
};

Thread* ThreadMap::GetThread(char threadId)
{
    auto it = threadMap.find(threadId);
    if (it != threadMap.cend())
    {
        return it->second;
    }
    Thread* thread = new Thread(threadId);
    threads.push_back(std::unique_ptr<Thread>(thread));
    threadMap[threadId] = thread;
    return thread;
}

std::string TimeStr(int64_t nanosecs)
{
    std::string timeStr;
    int64_t ns = nanosecs % 1000;
    int64_t mus = (nanosecs / 1000) % 1000;
    int64_t ms = (nanosecs / (1000ll * 1000)) % 1000;
    int64_t s = (nanosecs / (1000ll * 1000 * 1000)) % 60;
    int64_t m = (nanosecs / (1000ll * 1000 * 1000 * 60)) % 60;
    int64_t h = (nanosecs / (1000ll * 1000 * 1000 * 60 * 60));
    timeStr.append(std::to_string(h / 10)).append(std::to_string(h % 10)).append(1, ':').append(std::to_string(m / 10)).append(std::to_string(m % 10)).append(1, ':');
    timeStr.append(std::to_string(s / 10)).append(std::to_string(s % 10)).append(1, '.').append(std::to_string(ms / 100)).append(std::to_string((ms % 100) / 10)).append(std::to_string(ms % 10));
    timeStr.append(1, '.').append(std::to_string(mus / 100)).append(std::to_string((mus % 100) / 10).append(std::to_string(mus % 10)));
    timeStr.append(1, '.').append(std::to_string(ns / 100)).append(std::to_string((ns % 100) / 10).append(std::to_string(ns % 10)));
    return timeStr;
}

void ConvertTraceToText(const std::string& traceFilePath)
{
    ThreadMap threadMap;
    std::string textFilePath = Path::ChangeExtension(traceFilePath, ".txt");
    std::ofstream textFile(textFilePath);
    ReadTraceTable();
    std::vector<TraceEntry> trace = ReadTrace(traceFilePath);
    int32_t n = trace.size();
    for (int32_t i = 0; i < n; ++i)
    {
        TraceEntry& entry = trace[i];
        std::string entryStr;
        entryStr.append(TimeStr(entry.nanosecs)).append(1, ' ');
        Thread* thread = threadMap.GetThread(entry.threadId);
        entryStr.append(std::string(1, entry.threadId)).append(1, ' ');
        if (entry.kind == EntryKind::begin)
        {
            entryStr.append(thread->indent, ' ');
            entryStr.append(1, '>').append(TraceTable::Instance().GetTraceFunctionName(entry.id));
            while (thread->indent >= thread->startEntries.size())
            {
                thread->startEntries.push_back(nullptr);
            }
            thread->startEntries[thread->indent] = &entry;
            ++thread->indent;
        }
        else if (entry.kind == EntryKind::end)
        {
            if (thread->indent > 0)
            {
                --thread->indent;
            }
            entryStr.append(thread->indent, ' ');
            entryStr.append(1, '<').append(TraceTable::Instance().GetTraceFunctionName(entry.id));
            if (thread->startEntries.size() > thread->indent)
            {
                TraceEntry* start = thread->startEntries[thread->indent];
                if (start)
                {
                    if (start->id == entry.id)
                    {
                        int64_t duration = entry.nanosecs - start->nanosecs;
                        entryStr.append(" duration=").append(TimeStr(duration));
                        thread->startEntries[thread->indent] = nullptr;
                    }
                }
            }
        }
        else if (entry.kind == EntryKind::message)
        {
            entryStr.append(thread->indent, ' ');
            entryStr.append(1, '!').append(TraceTable::Instance().GetTraceMessageName(entry.id));
        }
        textFile << entryStr << std::endl;
    }
}

bool tracing = false;

std::chrono::time_point<std::chrono::steady_clock> start;

void BeginTracing()
{
    tracing = true;
    start = std::chrono::steady_clock::now();
}

void EndTracing()
{
    tracing = false;
    Trace::Instance().Write();
}

#ifdef _WIN32
__declspec(thread) char threadId = '0';
#else
__thread char* threadId = '0';
#endif


void SetThreadId(char threadId_)
{
    threadId = threadId_;
}

void AddTraceEntry(EntryKind kind, int32_t id)
{
    if (!tracing) return;
    std::chrono::steady_clock::duration time = std::chrono::steady_clock::now() - start;
    TraceEntry entry(kind, threadId, id, std::chrono::duration_cast<std::chrono::nanoseconds>(time).count());
    Trace::Instance().AddEntry(entry);
}

void InitTrace()
{
    Trace::Init();
    TraceTable::Init();
}

void DoneTrace()
{
    TraceTable::Done();
    Trace::Done();
}

} } // namespace soulng::util
