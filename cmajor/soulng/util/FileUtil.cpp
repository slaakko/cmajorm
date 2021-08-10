// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/FileUtil.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/BinaryStreamReader.hpp>
#include <soulng/util/BinaryStreamWriter.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace soulng { namespace util {

void CopyFile(const std::string& source, const std::string& dest, bool force, bool verbose)
{
    if (!boost::filesystem::exists(source))
    {
        if (verbose)
        {
            std::cout << "source file '" + source + "' does not exist" << std::endl;
        }
        return;
    }
    if (force || !boost::filesystem::exists(dest) || boost::filesystem::last_write_time(source) > boost::filesystem::last_write_time(dest))
    {
        int64_t size = boost::filesystem::file_size(source);
        {
            FileStream sourceFile(source, OpenMode::read | OpenMode::binary);
            BufferedStream bufferedSource(sourceFile);
            BinaryStreamReader reader(bufferedSource);
            FileStream destFile(dest, OpenMode::write | OpenMode::binary);
            BufferedStream bufferedDest(destFile);
            BinaryStreamWriter writer(bufferedDest);
            for (int64_t i = 0; i < size; ++i)
            {
                uint8_t x = reader.ReadByte();
                writer.Write(x);
            }
        }
        boost::system::error_code ec;
        boost::filesystem::last_write_time(dest, boost::filesystem::last_write_time(source), ec);
        if (ec)
        {
            throw std::runtime_error("could not set write time of file '" + dest + "': " + PlatformStringToUtf8(ec.message()));
        }
        if (verbose)
        {
            std::cout << source << " -> " << dest << std::endl;
        }
    }
}

} } // namespace soulng::util
