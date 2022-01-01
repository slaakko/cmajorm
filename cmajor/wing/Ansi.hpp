// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_ANSI_INCLUDED
#define CMAJOR_WING_ANSI_INCLUDED
#include <wing/WingApi.hpp>
#include <wing/Color.hpp>
#include <soulng/util/Unicode.hpp>
#include <functional>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

using ColorOutputFn = std::function<void(ConsoleColor, ConsoleColor, int, char32_t)>;

class ColorCharOutputMethod
{
public:
    template<class T>
    void Set(T* t, void (T::* pm)(ConsoleColor textColor, ConsoleColor backColor, int handle, char32_t c))
    {
        fn = std::bind(pm, t, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    }
    void operator()(ConsoleColor textColor, ConsoleColor backColor, int handle, char32_t c)
    {
        fn(textColor, backColor, handle, c);
    }
private:
    ColorOutputFn fn;
};

class AnsiEngine
{
public:
    AnsiEngine(int handle_);
    void SetColorCharOutputMethod(const ColorCharOutputMethod& outputMethod_);
    void Process(const std::string& text);
private:
    void Put(char c);
    void Emit(char c);
    void ResetColors();
    void SetColors();
    int handle;
    int state;
    ConsoleColor textColor;
    ConsoleColor backColor;
    ConsoleColor currentTextColor;
    ConsoleColor currentBackColor;
    ConsoleColor defaultTextColor;
    ConsoleColor defaultBackColor;
    Utf8ToUtf32Engine unicodeEngine;
    ColorCharOutputMethod outputMethod;
};

} } // cmajor::wing

#endif // CMAJOR_WING_ANSI_INCLUDED
