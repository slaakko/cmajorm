// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/Time.hpp>

namespace soulng { namespace util {

LogFileWriter::LogFileWriter(const std::string& logFilePath_, bool open_) : logFilePath(logFilePath_), open(open_)
{
    if (open)
    {
        std::ios_base::openmode mode = std::ios_base::app;
        logFile.open(logFilePath, mode);
    }
}

LogFileWriter::LogFileWriter(const std::string& logFilePath) : LogFileWriter(logFilePath, true)
{
}

LogFileWriter& LogFileWriter::operator<<(StandardEndLine manip)
{
    if (!open) return *this;
    WriteLine();
    return *this;
}

void LogFileWriter::WriteCurrentDateTime()
{
    if (!open) return;
    DateTime timestamp = GetCurrentDateTime();
    logFile << timestamp.ToString() << std::endl;
}

void LogFileWriter::WriteLine()
{
    if (!open) return;
    logFile << std::endl;
}

void LogFileWriter::WriteLine(const std::string& line)
{
    if (!open) return;
    logFile << line << std::endl;
}

void LogFileWriter::Write(const std::string& s)
{
    if (!open) return;
    logFile << s;
}

LogFileWriter& operator<<(LogFileWriter& writer, const std::string& s)
{
    writer.Write(s);
    return writer;
}

LogFileWriter& operator<<(LogFileWriter& writer, const char* s)
{
    writer.Write(s);
    return writer;
}

LogFileWriter& operator<<(LogFileWriter& writer, char c)
{
    writer.Write(std::string(1, c));
    return writer;
}

LogFileWriter& operator<<(LogFileWriter& writer, bool b)
{
    if (b)
    {
        writer.Write("true");
    }
    else
    {
        writer.Write("false");
    }
    return writer;

}

LogFileWriter& operator<<(LogFileWriter& writer, int x)
{
    writer.Write(std::to_string(x));
    return writer;
}

LogFileWriter& operator<<(LogFileWriter& writer, double x)
{
    writer.Write(std::to_string(x));
    return writer;
}

LogFileWriter& operator<<(LogFileWriter& writer, int64_t x)
{
    writer.Write(std::to_string(x));
    return writer;
}

LogFileWriter& operator<<(LogFileWriter& writer, uint64_t x)
{
    writer.Write(std::to_string(x));
    return writer;
}

} } // namespace soulng::util
