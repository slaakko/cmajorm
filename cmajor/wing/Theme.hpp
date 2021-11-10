// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_THEME_INCLUDED
#define CMAJOR_WING_THEME_INCLUDED
#include <wing/Graphics.hpp>
#include <vector>
#include <map>

namespace cmajor { namespace wing {

class Control;

class WING_API Theme
{
public:
    Theme(const std::string& name_, const std::string& filePath_);
    void Save();
    void Load();
    const std::string& Name() const { return name; }
    const std::string& FilePath() const { return filePath; }
    Color GetColor(const std::string& itemName) const;
    void SetColor(const std::string& itemName, const Color& color);
    std::string GetBitmapName(const std::string& itemName) const;
    void SetBitmapName(const std::string& itemName, const std::string& bitmapName);
private:
    std::string name;
    std::string filePath;
    std::map<std::string, Color> itemColorMap;
    std::map<std::string, std::string> itemBitmapNameMap;
};

WING_API std::string GetTheme();
WING_API void SetTheme(const std::string& themeName);
WING_API std::vector<std::string> GetThemes();
WING_API void AddTheme(Theme* theme);
WING_API bool ThemesFileExists();
WING_API void SaveThemes();
WING_API void LoadThemes();
WING_API void RegisterUpdateColorsListener(Control* control);
WING_API void UnregisterUpdateColorsListener(Control* control);
WING_API Color GetColor(const std::string& itemName);
WING_API std::string GetBitmapName(const std::string& itemName);
WING_API void ThemeInit();
WING_API void ThemeDone();

} } // namespace cmajor::wing

#endif // CMAJOR_WING_THEME_INCLUDED
