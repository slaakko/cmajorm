// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CMDB_SESSION_INCLUDED
#define CMAJOR_DEBUG_CMDB_SESSION_INCLUDED
#include <cmajor/cmdebug/DebuggerOutputWriter.hpp>

namespace cmajor { namespace debug {

class DEBUG_API RemoveCmdbSessionFileGuard
{
public:
    RemoveCmdbSessionFileGuard(const std::string& cmdbSessionFilePath_); 
    ~RemoveCmdbSessionFileGuard();
private:
    std::string cmdbSessionFilePath;
};

class DEBUG_API CmdbSessionClient
{
public:
    virtual ~CmdbSessionClient();
    virtual std::string GetTargetInputBytes() = 0;
    virtual void WriteTargetOuput(int handle, const std::string& s) = 0;
};

DEBUG_API void SetCmdbSessionPort(int port_);
DEBUG_API void StartCmdbSession(const std::string& cmdbSessionFilePath, DebuggerOutputWriter* outputWriter, CmdbSessionClient* client, bool verbose);
DEBUG_API void StopCmdbSession();
DEBUG_API void CmdbSessionInit();
DEBUG_API void CmdbSessionDone();

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CMDB_SESSION_INCLUDED
