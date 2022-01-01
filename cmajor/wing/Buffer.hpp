// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_BUFFER_INCLUDED
#define CMAJOR_WING_BUFFER_INCLUDED
#include <wing/Graphics.hpp>

namespace cmajor { namespace wing {

class Buffer
{
public:
    Buffer(const Size& size_, Graphics& graphics_);
    const Size& GetSize() const { return size; }
    Graphics& BitmapGraphics() { return bmgraphics; }
    Bitmap& GetBitmap() { return bitmap; }
    void Draw(Graphics& graphics);
private:
    Size size;
    Bitmap bitmap;
    Graphics bmgraphics;
};

} } // cmajor::wing

#endif // CMAJOR_WING_BUFFER_INCLUDED
