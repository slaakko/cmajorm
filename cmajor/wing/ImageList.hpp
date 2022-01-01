// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_IMAGE_LIST_INCLUDED
#define CMAJOR_WING_IMAGE_LIST_INCLUDED
#include <wing/Graphics.hpp>
#include <map>
#include <vector>

namespace cmajor { namespace wing {

class WING_API ImageList
{
public:
    ImageList();
    ImageList(const ImageList&) = delete;
    ImageList operator=(const ImageList&) = delete;
    void AddImage(const std::string& imageName);
    void AddDisabledImage(const std::string& imageName);
    void AddImage(const std::string& imageName, Bitmap* bitmap);
    int GetImageIndex(const std::string& imageName) const;
    int GetDisabledImageIndex(const std::string& imageName) const;
    Bitmap* GetImage(int imageIndex) const;
private:
    std::map<std::string, int> imageIndexMap;
    std::vector<std::unique_ptr<Bitmap>> images;
};

} } // cmajor::wing

#endif // CMAJOR_WING_IMAGE_LIST_INCLUDED
