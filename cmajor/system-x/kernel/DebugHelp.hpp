// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_DEBUG_HELP_INCLUDED
#define CMSX_KERNEL_DEBUG_HELP_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Event.hpp>
#include <mutex>

namespace cmsx::kernel {

#define LOCK_DEBUG 0

const int BLOCK_MANAGER = 1;
const int EVENT_MANAGER = 2;
const int HOST_FILE_MANAGER = 3;
const int HOST_FILE_SYSTEM = 4;
const int INODE_MANAGER = 5;
const int IO_MANAGER = 6;
const int KERNEL = 7;
const int PROCESS_MANAGER = 8;
const int ROOT_FILE_SYSTEM = 9;
const int SCHEDULER = 10;
const int PIPE = 11;
const int DEV_FILE_SYSTEM = 12;
const int MSG_QUEUE = 13;

const int NO_LOCK = 0;
const int HAS_LOCK = 1;

const int SLEEP = 1 << 1;
const int WAKEUP = 2 << 1;
const int GET_BLOCK = 3 << 1;
const int PUT_BLOCK = 4 << 1;
const int OPEN_FILE = 5 << 1;
const int CLOSE_FILE = 6 << 1;
const int GET_HOST_FILE = 7 << 1;
const int READ = 8 << 1;
const int WRITE = 9 << 1;
const int OPEN = 10 << 1;
const int CLOSE = 11 << 1;
const int SEARCH_DIRECTORY = 12 << 1;
const int OPEN_DIR = 13 << 1;
const int CLOSE_DIR = 14 << 1;
const int MK_DIR = 15 << 1;
const int GET_HOST_FILE_PATH = 16 << 1;
const int READ_INODE = 17 << 1;
const int CLEAR_PROCESS_DATA = 18 << 1;
const int GET_INODE = 19 << 1;
const int PUT_INODE = 20 << 1;
const int HANDLE_REQUEST = 21 << 1;
const int HANDLE_COMPLETION = 22 << 1;
const int GET_REQUEST = 23 << 1;
const int DELETE_REQUEST = 24 << 1;
const int FORK = 25 << 1;
const int WAIT = 26 << 1;
const int EXIT = 27 << 1;
const int CREATE_PROCESS = 28 << 1;
const int DELETE_PROCESS = 29 << 1;
const int WAIT_FOR_EXIT = 30 << 1;
const int GET_RUNNABLE_PROCESS = 31 << 1;
const int ADD_RUNNABLE_PROCESS = 32 << 1;
const int CHECK_RUNNABLE = 33 << 1;
const int SET_FILE_PATH_PROCESS = 34 << 1;

CMSX_KERNEL_API void DebugBreak();

CMSX_KERNEL_API std::string SiteStr(int site);

class CMSX_KERNEL_API DebugLock
{
public:
    DebugLock(std::recursive_mutex* machineLock_, int site_, int processId_, int flags_);
    DebugLock(std::recursive_mutex* machineLock_, int site_, int processId_, int flags_, const std::string& str_);
    DebugLock(std::recursive_mutex* machineLock_, int site_, int processId_, int flags_, const cmsx::machine::Event& evnt_);
    ~DebugLock();
private:
    std::recursive_mutex* machineLock;
    int site;
    int processId;
    int flags;
    cmsx::machine::Event evnt;
    std::string str;
    bool logged;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_DEBUG_HELP_INCLUDED
