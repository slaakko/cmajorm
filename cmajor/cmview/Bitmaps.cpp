// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/Bitmaps.hpp>
#include <soulng/util/Unicode.hpp>
#include <memory>
#include <map>
#include <vector>

namespace cmajor { namespace view {

using namespace soulng::unicode;

class Bitmaps
{
public:
    static void Init();
    static void Done();
    static Bitmaps& Instance() { return *instance; }
    Bitmap* GetBitmap(const std::string& bitmapName);
    void Clear();
private:
    static std::unique_ptr<Bitmaps> instance;
    std::vector<std::unique_ptr<Bitmap>> bitmaps;
    std::map<std::string, Bitmap*> bitmapMap;
};

std::unique_ptr<Bitmaps> Bitmaps::instance;

void Bitmaps::Init()
{
    instance.reset(new Bitmaps());
}

void Bitmaps::Done()
{
    instance.reset();
}

Bitmap* Bitmaps::GetBitmap(const std::string& bitmapName)
{
    auto it = bitmapMap.find(bitmapName);
    if (it != bitmapMap.cend())
    {
        return it->second;
    }
    else
    {
        std::u16string u16bitmapName = ToUtf16(bitmapName);
        std::unique_ptr<Bitmap> bitmapPtr(Bitmap::FromResource(cmajor::wing::Instance(), (const WCHAR*)u16bitmapName.c_str()));
        Bitmap* bitmap = bitmapPtr.get();
        bitmapMap[bitmapName] = bitmap;
        bitmaps.push_back(std::move(bitmapPtr));
        return bitmap;
    }
}

void Bitmaps::Clear()
{
    bitmaps.clear();
    bitmapMap.clear();
}

Bitmap* GetBitmap(const std::string& bitmapName)
{
    return Bitmaps::Instance().GetBitmap(bitmapName);
}

void ClearBitmaps()
{
    Bitmaps::Instance().Clear();
}

void InitBitmaps()
{
    Bitmaps::Init();
}

void DoneBitmaps()
{
    Bitmaps::Done();
}

} } // namespace cmajor::view
