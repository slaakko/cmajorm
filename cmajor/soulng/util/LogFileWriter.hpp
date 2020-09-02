// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_LOG_FILE_INCLUDED
#define SOULNG_UTIL_LOG_FILE_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>
#include <fstream>

namespace soulng { namespace util {

class UTIL_API LogFileWriter
{
public:
    LogFileWriter(const std::string& logFilePath_, bool open_);
    LogFileWriter(const std::string& logFilePath);
    void WriteCurrentDateTime();
    void WriteLine();
    void WriteLine(const std::string& line);
    void Write(const std::string& s);
    typedef std::basic_ostream<char, std::char_traits<char>> CoutType;
    typedef CoutType& (*StandardEndLine)(CoutType&);
    LogFileWriter& operator<<(StandardEndLine manip);
    std::ostream& LogFile() { return logFile; }
private:
    bool open;
    std::string logFilePath;
    std::ofstream logFile;
};

UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, const std::string& s);
UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, const char* s);
UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, char c);
UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, bool b);
UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, int x);
UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, double x);
UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, int64_t x);
UTIL_API LogFileWriter& operator<<(LogFileWriter& writer, uint64_t x);

} } // namespace soulng::util

#endif // SOULNG_UTIL_LOG_FILE_INCLUDED
