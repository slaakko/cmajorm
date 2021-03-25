// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/FileLocking.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <stdexcept>

namespace soulng { namespace util {

class MappedInputFileImpl
{
public:
    MappedInputFileImpl(const std::string& fileName);
    const char* Data() const { return mappedFile.const_data(); }
    boost::iostreams::mapped_file_source::size_type Size() const { return mappedFile.size(); }
private:
    boost::iostreams::mapped_file mappedFile;
};

MappedInputFileImpl::MappedInputFileImpl(const std::string& fileName) : mappedFile()
{
    try
    {
        LockFile(fileName, LockKind::read);
        mappedFile.open(fileName, boost::iostreams::mapped_file::mapmode::readonly);
    }
    catch (std::exception& ex)
    {
        throw std::runtime_error("error opening mapped file '" + fileName + "': " + ex.what());
    }
    catch (...)
    {
        throw std::runtime_error("error opening mapped file '" + fileName + "'");
    }
}

MappedInputFile::MappedInputFile(const std::string& fileName_) : fileName(fileName_), impl(new MappedInputFileImpl(fileName))
{
}

MappedInputFile::~MappedInputFile()
{
    delete impl;
    UnlockFile(fileName, LockKind::read);
}

const char* MappedInputFile::Begin() const
{
    const char* start = impl->Data();
    if (impl->Size() >= 3)
    {
        if ((unsigned char)start[0] == (unsigned char)0xEF &&
            (unsigned char)start[1] == (unsigned char)0xBB &&
            (unsigned char)start[2] == (unsigned char)0xBF)
        {
            start += 3;
        }
    }
    return start;
}

const char* MappedInputFile::End() const
{
    return impl->Data() + impl->Size();
}

std::string ReadFile(const std::string& fileName)
{
    if (!boost::filesystem::exists(fileName))
    {
        throw std::runtime_error("file '" + fileName + "' does not exist");
    }
    if (boost::filesystem::file_size(fileName) == 0)
    {
        return std::string();
    }
    MappedInputFile mappedFile(fileName);
    return std::string(mappedFile.Begin(), mappedFile.End());
}

} } // namespace soulng::util
