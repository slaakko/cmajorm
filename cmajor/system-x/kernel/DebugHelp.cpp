// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/DebugHelp.hpp>
#include <system-x/kernel/File.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/machine/Event.hpp>
#include <boost/filesystem.hpp>

namespace cmsx::kernel {

    void DebugBreak()
    {
        std::vector<int> x;
        if (boost::filesystem::exists("foo"))
        {
            x.push_back(1);
        }
    }

    std::string SiteStr(int site)
    {
        switch (site)
        {
        case BLOCK_MANAGER:
        {
            return "BLOCK_MANAGER";
        }
        case EVENT_MANAGER:
        {
            return "EVENT_MANAGER";
        }
        case HOST_FILE_MANAGER:
        {
            return "HOST_FILE_MANAGER";
        }
        case HOST_FILE_SYSTEM:
        {
            return "HOST_FILE_SYSTEM";
        }
        case INODE_MANAGER:
        {
            return "INODE_MANAGER";
        }
        case IO_MANAGER:
        {
            return "IO_MANAGER";
        }
        case KERNEL:
        {
            return "KERNEL";
        }
        case PROCESS_MANAGER:
        {
            return "PROCESS_MANAGER";
        }
        case ROOT_FILE_SYSTEM:
        {
            return "ROOT_FILE_SYSTEM";
        }
        case SCHEDULER:
        {
            return "SCHEDULER";
        }
        default:
        {
            return "UNKNOWN_SITE";
        }
        }
    }

bool HasFlags(int flags, int op)
{
    if (((flags & 0xFFFFFFFE) >> 1) << 1 == op)
    {
        return true;
    }
    return false;
}

std::string MakeLogEntry(bool start, int site, int processId, int flags)
{
    std::string logEntry;
    if (start)
    {
        logEntry.append(">>>").append(SiteStr(site));
    }
    else
    {
        logEntry.append("<<<").append(SiteStr(site));
    }
    logEntry.append(", PID=").append(std::to_string(processId));
    if ((flags & HAS_LOCK) != 0)
    {
        logEntry.append(", LOCK(1):");
    }
    else
    {
        logEntry.append(", LOCK(0):");
    }
    switch (((flags & 0xFFFFFFFE) >> 1) << 1)
    {
        case SLEEP:
        {
            logEntry.append("SLEEP");
            break;
        }
        case WAKEUP:
        {
            logEntry.append("WAKEUP");
            break;
        }
        case GET_BLOCK:
        {
            logEntry.append("GET_BLOCK");
            break;
        }
        case PUT_BLOCK:
        {
            logEntry.append("PUT_BLOCK");
            break;
        }
        case OPEN_FILE:
        {
            logEntry.append("OPEN_FILE");
            break;
        }
        case CLOSE_FILE:
        {
            logEntry.append("CLOSE_FILE");
            break;
        }
        case GET_HOST_FILE:
        {
            logEntry.append("GET_HOST_FILE");
            break;
        }
        case READ:
        {
            logEntry.append("READ");
            break;
        }
        case WRITE:
        {
            logEntry.append("WRITE");
            break;
        }
        case OPEN:
        {
            logEntry.append("GET_BLOCK");
            break;
        }
        case CLOSE:
        {
            logEntry.append("CLOSE");
            break;
        }
        case SEARCH_DIRECTORY:
        {
            logEntry.append("SEARCH_DIRECTORY");
            break;
        }
        case OPEN_DIR:
        {
            logEntry.append("OPEN_DIR");
            break;
        }
        case CLOSE_DIR:
        {
            logEntry.append("CLOSE_DIR");
            break;
        }
        case MK_DIR:
        {
            logEntry.append("MK_DIR");
            break;
        }
        case GET_HOST_FILE_PATH:
        {
            logEntry.append("GET_HOST_FILE_PATH");
            break;
        }
        case READ_INODE:
        {
            logEntry.append("READ_INODE");
            break;
        }
        case CLEAR_PROCESS_DATA:
        {
            logEntry.append("CLEAR_PROCESS_DATA");
            break;
        }
        case GET_INODE:
        {
            logEntry.append("GET_INODE");
            break;
        }
        case PUT_INODE:
        {
            logEntry.append("PUT_INODE");
            break;
        }
        case HANDLE_REQUEST:
        {
            logEntry.append("HANDLE_REQUEST");
            break;
        }
        case HANDLE_COMPLETION:
        {
            logEntry.append("HANDLE_COMPLETION");
            break;
        }
        case GET_REQUEST:
        {
            logEntry.append("GET_REQUEST");
            break;
        }
        case DELETE_REQUEST:
        {
            logEntry.append("DELETE_REQUEST");
            break;
        }
        case FORK:
        {
            logEntry.append("FORK");
            break;
        }
        case WAIT:
        {
            logEntry.append("WAIT");
            break;
        }
        case EXIT:
        {
            logEntry.append("EXIT");
            break;
        }
        case CREATE_PROCESS:
        {
            logEntry.append("CREATE_PROCESS");
            break;
        }
        case DELETE_PROCESS:
        {
            logEntry.append("DELETE_PROCESS");
            break;
        }
        case WAIT_FOR_EXIT:
        {
            logEntry.append("WAIT_FOR_EXIT");
            break;
        }
        case GET_RUNNABLE_PROCESS:
        {
            logEntry.append("GET_RUNNABLE_PROCESS");
            break;
        }
        case ADD_RUNNABLE_PROCESS:
        {
            logEntry.append("ADD_RUNNABLE_PROCESS");
            break;
        }
        case CHECK_RUNNABLE:
        {
            logEntry.append("CHECK_RUNNABLE");
            break;
        }
        case SET_FILE_PATH_PROCESS:
        {
            logEntry.append("SET_FILE_PATH_PROCESS");
            break;
        }
    }
    return logEntry;
}

DebugLock::DebugLock(std::recursive_mutex* machineLock_, int site_, int processId_, int flags_, const cmsx::machine::Event& evnt_) :
    machineLock(machineLock_), site(site_), processId(processId_), flags(flags_), evnt(evnt_), str(), logged(false)
{
    if (evnt.kind != cmsx::machine::EventKind::blockFreeEvent) return;
    if (evnt.id == 0) return;
    //if (processId != 5 && processId != 6 && processId != 7) return;
    if (HasFlags(flags, SLEEP) || HasFlags(flags, WAKEUP))
    {
        std::string logEntry = MakeLogEntry(true, site, processId, flags);
        logEntry.append(":").append(evnt.ToString());
        logEntry.append("\n");
        WriteToTerminal(logEntry, cmsx::kernel::Kernel::Instance().GetKernelProcess());
        logged = true;
    }
}

DebugLock::DebugLock(std::recursive_mutex* machineLock_, int site_, int processId_, int flags_, const std::string& str_) :
    machineLock(machineLock_), site(site_), processId(processId_), flags(flags_), evnt(), str(str_), logged(false)
{
    //if (processId != 5 && processId != 6 && processId != 7) return;
    if (HasFlags(flags, SLEEP) || HasFlags(flags, WAKEUP))
    {
        std::string logEntry = MakeLogEntry(true, site, processId, flags);
        logEntry.append(":").append(str);
        logEntry.append("\n");
        WriteToTerminal(logEntry, cmsx::kernel::Kernel::Instance().GetKernelProcess());
        logged = true;
    }
}

DebugLock::DebugLock(std::recursive_mutex* machineLock_, int site_, int processId_, int flags_) :
    machineLock(machineLock_), site(site_), processId(processId_), flags(flags_), evnt(), str(), logged(false)
{
    if (evnt.kind != cmsx::machine::EventKind::blockFreeEvent) return;
    //if (processId != 5 && processId != 6 && processId != 7) return;
    if (HasFlags(flags, SLEEP) || HasFlags(flags, WAKEUP))
    {
        std::string logEntry = MakeLogEntry(true, site, processId, flags);
        logEntry.append("\n");
        WriteToTerminal(logEntry, cmsx::kernel::Kernel::Instance().GetKernelProcess());
        logged = true;
    }
}

DebugLock::~DebugLock()
{
    if (!logged)
    {
        return;
    }
    std::string logEntry = MakeLogEntry(false, site, processId, flags);
    if (evnt != cmsx::machine::Event())
    {
        logEntry.append(":").append(evnt.ToString());
    }
    logEntry.append("\n");
    WriteToTerminal(logEntry, cmsx::kernel::Kernel::Instance().GetKernelProcess());
}

} // namespace cmsx::kernel
