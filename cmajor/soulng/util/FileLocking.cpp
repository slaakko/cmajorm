// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/FileLocking.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Path.hpp>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <map>
#include <set>

namespace soulng { namespace util {

bool debugFileLocking = true;

class FileLockTable
{
public:
    static void Init();
    static void Done();
    static FileLockTable& Instance() { return *instance; }
    void LockFile(const std::string& filePath, LockKind lockKind);
    void UnlockFile(const std::string& filePath, LockKind lockKind);
    bool NoReaders(const std::string& filePath);
    bool NoWriters(const std::string& filePath);
private:
    static std::unique_ptr<FileLockTable> instance;
    FileLockTable();
    std::mutex mtx;
    std::map<std::string, int> readMap;
    std::set<std::string> writeLocks;
    std::condition_variable releaseRead;
    std::condition_variable releaseWrite;
    bool waitingReadLock;
    bool waitingWriteLock;
};

std::unique_ptr<FileLockTable> FileLockTable::instance;

FileLockTable::FileLockTable() : waitingReadLock(false), waitingWriteLock(false)
{
}

void FileLockTable::Init()
{
    instance.reset(new FileLockTable());
}

void FileLockTable::Done()
{
    instance.reset();
}

void FileLockTable::LockFile(const std::string& filePath, LockKind lockKind)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (lockKind == LockKind::write)
    {
        auto itRead = readMap.find(filePath);
        if (itRead != readMap.cend())
        {
            waitingReadLock = true;
            if (debugFileLocking)
            {
                LogMessage(-1, "File '" + filePath + " locked for reading, number of readers is " + std::to_string(itRead->second));
            }
            releaseRead.wait(lock, [this, filePath] { return NoReaders(filePath); });
            if (debugFileLocking)
            {
                LogMessage(-1, "File '" + filePath + "': readers exited");
            }
            waitingReadLock = false;
        }
        auto itWrite = writeLocks.find(filePath);
        if (itWrite != writeLocks.cend())
        {
            waitingWriteLock = true;
            if (debugFileLocking)
            {
                LogMessage(-1, "File '" + filePath + " locked for writing");
            }
            releaseWrite.wait(lock, [this, filePath] { return NoWriters(filePath); });
            if (debugFileLocking)
            {
                LogMessage(-1, "File '" + filePath + ": writer exited");
            }
            waitingWriteLock = false;
        }
    }
    else if (lockKind == LockKind::read)
    {
        auto itWrite = writeLocks.find(filePath);
        if (itWrite != writeLocks.cend())
        {
            waitingWriteLock = true;
            if (debugFileLocking)
            {
                LogMessage(-1, "File '" + filePath + " locked for writing");
            }
            releaseWrite.wait(lock, [this, filePath] { return NoWriters(filePath); });
            if (debugFileLocking)
            {
                LogMessage(-1, "File '" + filePath + ": writer exited");
            }
            waitingWriteLock = false;
        }
    }
    if (lockKind == LockKind::write)
    {
        writeLocks.insert(filePath);
    }
    else if (lockKind == LockKind::read)
    {
        ++readMap[filePath];
    }
}

bool FileLockTable::NoReaders(const std::string & filePath)
{
    auto it = readMap.find(filePath);
    if (it != readMap.end())
    {
        int numReaders = it->second;
        return numReaders == 0;
    }
    return true;
}

bool FileLockTable::NoWriters(const std::string& filePath)
{
    return writeLocks.find(filePath) == writeLocks.cend();
}

void FileLockTable::UnlockFile(const std::string& filePath, LockKind lockKind)
{
    std::unique_lock<std::mutex> lock(mtx);
    if (lockKind == LockKind::read)
    {
        int& numReaders = readMap[filePath];
        --numReaders;
        if (numReaders == 0)
        {
            readMap.erase(filePath);
            if (waitingReadLock)
            {
                releaseRead.notify_all();
            }
        }
    }
    else if (lockKind == LockKind::write)
    {
        writeLocks.erase(filePath);
        if (waitingWriteLock)
        {
            releaseWrite.notify_all();
        }
    }
}

void LockFile(const std::string& filePath, LockKind lockKind)
{
    FileLockTable::Instance().LockFile(GetFullPath(filePath), lockKind);
}

void UnlockFile(const std::string& filePath, LockKind lockKind)
{
    FileLockTable::Instance().UnlockFile(GetFullPath(filePath), lockKind);
}

void InitFileLocking()
{
    FileLockTable::Init();
}

void DoneFileLocking()
{
    FileLockTable::Done();
}

} } // namespace soulng::util
