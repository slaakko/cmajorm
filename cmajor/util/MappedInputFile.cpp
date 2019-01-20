// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/util/MappedInputFile.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace cmajor { namespace util {

class MappedInputFileImpl
{
public:
    MappedInputFileImpl(const std::string& fileName_);
    const char* Data() const { return mappedFile.const_data(); }
    boost::iostreams::mapped_file_source::size_type Size() const { return mappedFile.size(); }
private:
    boost::iostreams::mapped_file mappedFile;
};

MappedInputFileImpl::MappedInputFileImpl(const std::string& fileName_) : mappedFile()
{
    try
    {
        mappedFile.open(fileName_, boost::iostreams::mapped_file::mapmode::readonly);
    }
    catch (std::exception& ex)
    {
        throw std::runtime_error("error opening mapped file '" + fileName_ + "': " + ex.what());
    }
    catch (...)
    {
        throw std::runtime_error("error opening mapped file '" + fileName_ + "'");
    }
}

MappedInputFile::MappedInputFile(const std::string& fileName_) : impl(new MappedInputFileImpl(fileName_))
{
}

MappedInputFile::~MappedInputFile()
{
    delete impl;
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
    MappedInputFile mappedFile(fileName);
    return std::string(mappedFile.Begin(), mappedFile.End());
}

} } // namespace cmajor::util
