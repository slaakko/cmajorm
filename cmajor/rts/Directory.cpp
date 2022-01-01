// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Directory.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <boost/filesystem.hpp>

namespace cmajor { namespace rt {

using namespace soulng::util;

struct Iteration
{
    boost::filesystem::directory_iterator directoryIterator;
    std::string directoryName;
    std::string path;
};

class DirectoryIterationTable
{
public:
    static void Init();
    static void Done();
    static DirectoryIterationTable& Instance() { return *instance; }
    int32_t BeginIterate(const char* directoryPath);
    const char* IterateFiles(int32_t handle);
    const char* IterateDirectories(int32_t handle);
    void EndIterate(int32_t handle);
private:
    static std::unique_ptr<DirectoryIterationTable> instance;
    DirectoryIterationTable();
    int32_t nextIterationHandle;
    std::unordered_map<int32_t, Iteration> iterationMap;
    std::mutex mtx;
};

std::unique_ptr<DirectoryIterationTable> DirectoryIterationTable::instance;

void DirectoryIterationTable::Init()
{
    instance.reset(new DirectoryIterationTable());
}

void DirectoryIterationTable::Done()
{
    instance.reset();
}

DirectoryIterationTable::DirectoryIterationTable() : nextIterationHandle(0)
{
}

int32_t DirectoryIterationTable::BeginIterate(const char* directoryPath)
{
    std::lock_guard<std::mutex> lock(mtx);
    int32_t handle = nextIterationHandle++;
    Iteration iteration;
    iteration.directoryName = GetFullPath(Path::MakeCanonical(directoryPath));
    iteration.directoryIterator = boost::filesystem::directory_iterator(soulng::util::MakeNativeBoostPath(iteration.directoryName));
    iterationMap[handle] = iteration;
    return handle;
}

void DirectoryIterationTable::EndIterate(int32_t handle)
{
    std::lock_guard<std::mutex> lock(mtx);
    iterationMap.erase(handle);
}

const char* DirectoryIterationTable::IterateFiles(int32_t handle)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = iterationMap.find(handle);
    if (it != iterationMap.cend())
    {
        Iteration& iteration = it->second;
        while (iteration.directoryIterator != boost::filesystem::directory_iterator() && !boost::filesystem::is_regular_file(*iteration.directoryIterator))
        {
            ++iteration.directoryIterator;
        }
        if (iteration.directoryIterator != boost::filesystem::directory_iterator())
        {
            iteration.path = soulng::util::PlatformStringToUtf8(GetFullPath(Path::Combine(
                soulng::util::Utf8StringToPlatformString(iteration.directoryName),
                boost::filesystem::path(*iteration.directoryIterator).generic_string())));
            ++iteration.directoryIterator;
            return iteration.path.c_str();
        }
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

const char* DirectoryIterationTable::IterateDirectories(int32_t handle)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = iterationMap.find(handle);
    if (it != iterationMap.cend())
    {
        Iteration& iteration = it->second;
        while (iteration.directoryIterator != boost::filesystem::directory_iterator() &&
            (!boost::filesystem::is_directory(*iteration.directoryIterator) || iteration.directoryIterator->path() == "." || iteration.directoryIterator->path() == ".."))
        {
            ++iteration.directoryIterator;
        }
        if (iteration.directoryIterator != boost::filesystem::directory_iterator())
        {
            iteration.path = soulng::util::PlatformStringToUtf8(GetFullPath(Path::Combine(
                soulng::util::Utf8StringToPlatformString(iteration.directoryName),
                boost::filesystem::path(*iteration.directoryIterator).generic_string())));
            ++iteration.directoryIterator;
            return iteration.path.c_str();
        }
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

void InitDirectory()
{
    DirectoryIterationTable::Init();
}

void DoneDirectory()
{
    DirectoryIterationTable::Done();
}

} } // namespace cmajor::rt

extern "C" RT_API bool RtDirectoryExists(const char* directoryPath)
{
    return boost::filesystem::exists(directoryPath);
}

extern "C" RT_API void RtCreateDirectories(const char* directoryPath)
{
    boost::filesystem::create_directories(directoryPath);
}

extern "C" RT_API int32_t RtBeginIterateDirectory(const char* directoryPath)
{
    return cmajor::rt::DirectoryIterationTable::Instance().BeginIterate(directoryPath);
}

extern "C" RT_API const char* RtGetNextFilePath(int32_t directoryIterationHandle)
{
    return cmajor::rt::DirectoryIterationTable::Instance().IterateFiles(directoryIterationHandle);
}

extern "C" RT_API const char* RtGetNextDirectoryPath(int32_t directoryIterationHandle)
{
    return cmajor::rt::DirectoryIterationTable::Instance().IterateDirectories(directoryIterationHandle);
}

extern "C" RT_API void RtEndIterateDirectory(int32_t directoryIterationHandle)
{
    cmajor::rt::DirectoryIterationTable::Instance().EndIterate(directoryIterationHandle);
}
