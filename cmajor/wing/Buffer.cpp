// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Buffer.hpp>

namespace cmajor { namespace wing {

Buffer::Buffer(const Size& size_, Graphics& graphics_) : size(size_), bitmap(size.Width, size.Height, &graphics_), bmgraphics(&bitmap)
{
}

void Buffer::Draw(Graphics& graphics)
{
    Point topLeftCorner;
    graphics.DrawImage(&bitmap, topLeftCorner);
}

} } // cmajor::wing