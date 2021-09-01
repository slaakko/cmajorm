// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ImageList.hpp>
#include <wing/Control.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

ImageList::ImageList()
{
}

void ImageList::AddImage(const std::string& imageName)
{
    std::u16string bitmapName = ToUtf16(imageName);
    Bitmap* bitmap = Bitmap::FromResource(Instance(), (const WCHAR*)bitmapName.c_str());
    AddImage(imageName, bitmap);
}

void ImageList::AddDisabledImage(const std::string& imageName)
{
    int imageIndex = GetImageIndex(imageName);
    if (imageIndex != -1)
    {
        Bitmap* bitmap = images[imageIndex].get();
        std::unique_ptr<Bitmap> disabledBitmap = ToGrayBitmap(bitmap, DefaultBitmapTransparentColor());
        AddImage(imageName + ".disabled", disabledBitmap.release());
    }
    else
    {
        throw std::runtime_error("image '" + imageName + "' not found");
    }
}

void ImageList::AddImage(const std::string& imageName, Bitmap* bitmap)
{
    int imageIndex = images.size();
    images.push_back(std::unique_ptr<Bitmap>(bitmap));
    imageIndexMap[imageName] = imageIndex;
}

int ImageList::GetImageIndex(const std::string& imageName) const
{
    auto it = imageIndexMap.find(imageName);
    if (it != imageIndexMap.cend())
    {
        return it->second;
    }
    else
    {
        return -1;
    }
}

int ImageList::GetDisabledImageIndex(const std::string& imageName) const
{
    return GetImageIndex(imageName + ".disabled");
}

Bitmap* ImageList::GetImage(int imageIndex) const
{
    if (imageIndex >= 0 && imageIndex < images.size())
    {
        return images[imageIndex].get();
    }
    else
    {
        return nullptr;
    }
}

} } // cmajor::wing

