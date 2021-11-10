// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Theme.hpp>
#include <wing/Control.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <set>

namespace cmajor { namespace wing {

using namespace soulng::unicode;
using namespace soulng::util;

bool IsHexNibble(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool IsHexByte(std::string& s)
{
    return s.length() == 2 && IsHexNibble(s[0]) && IsHexNibble(s[1]);
}

void ThrowInvalidColorString(const std::string& itemName)
{
    throw std::runtime_error("invalid item '" + itemName + "' color attribute: not in format '#RRGGBB' where RR, GG and BB are red, green and blue color values in hex");
}

std::string ToColorHexString(const Color& color)
{
    uint8_t r = color.GetRed();
    uint8_t g = color.GetGreen();
    uint8_t b = color.GetBlue();
    return "#" + ToHexString(r) + ToHexString(g) + ToHexString(b);
}

Color ParseColorHexString(const std::string& colorHexString, const std::string& itemName)
{
    std::string rr;
    std::string gg;
    std::string bb;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    std::string chs;
    if (colorHexString[0] == '#')
    {
        chs = colorHexString.substr(1);
    }
    else
    {
        ThrowInvalidColorString(itemName);
    }
    if (chs.length() >= 6)
    {
        rr = chs.substr(0, 2);
        gg = chs.substr(2, 2);
        bb = chs.substr(4, 2);
        if (!IsHexByte(rr) || !IsHexByte(gg) || !IsHexByte(bb))
        {
            ThrowInvalidColorString(itemName);
        }
    }
    else
    {
        ThrowInvalidColorString(itemName);
    }
    r = ParseHexByte(rr);
    g = ParseHexByte(gg);
    b = ParseHexByte(bb);
    Color color(r, g, b);
    return color;
}

std::string ConfigDir()
{
    std::string cmajorRoot = GetFullPath(CmajorRoot());
    std::string configDir = Path::Combine(cmajorRoot, "config");
    return configDir;
}

std::string GetThemesFilePath()
{
    std::string themesFilePath = Path::Combine(ConfigDir(), "themes.xml");
    return themesFilePath;
}

Theme::Theme(const std::string& name_, const std::string& filePath_) : name(name_), filePath(filePath_)
{
}

void Theme::Save()
{
    sngxml::dom::Document themeDoc;
    sngxml::dom::Element* root = new sngxml::dom::Element(U"theme");
    root->SetAttribute(U"name", ToUtf32(name));
    for (const auto& p : itemColorMap)
    {
        const std::string& name = p.first;
        const Color& color = p.second;
        sngxml::dom::Element* item = new sngxml::dom::Element(U"item");
        item->SetAttribute(U"name", ToUtf32(name));
        item->SetAttribute(U"color", ToUtf32(ToColorHexString(color)));
        root->AppendChild(std::unique_ptr<sngxml::dom::Node>(item));
    }
    for (const auto& p : itemBitmapNameMap)
    {
        const std::string& itemName = p.first;
        const std::string& bitmapName = p.second;
        sngxml::dom::Element* item = new sngxml::dom::Element(U"item");
        item->SetAttribute(U"name", ToUtf32(itemName));
        item->SetAttribute(U"bitmap", ToUtf32(bitmapName));
        root->AppendChild(std::unique_ptr<sngxml::dom::Node>(item));
    }
    themeDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(root));
    std::ofstream file(GetFullPath(Path::Combine(GetFullPath(ConfigDir()), filePath)));
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    themeDoc.Write(formatter);
}

void Theme::Load()
{
    try
    {
        std::unique_ptr<sngxml::dom::Document> themeDoc = sngxml::dom::ReadDocument(GetFullPath(Path::Combine(GetFullPath(ConfigDir()), filePath)));
        std::unique_ptr<sngxml::xpath::XPathObject> itemObject = sngxml::xpath::Evaluate(U"/theme/item", themeDoc.get());
        if (itemObject)
        {
            if (itemObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(itemObject.get());
                int n = nodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    sngxml::dom::Node* node = (*nodeSet)[i];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                        std::u32string itemNameAttr = element->GetAttribute(U"name");
                        if (itemNameAttr.empty())
                        {
                            throw std::runtime_error("item element " + std::to_string(i) + " has no 'name' attribute");
                        }
                        std::string itemName = ToUtf8(itemNameAttr);
                        std::u32string colorAttr = element->GetAttribute(U"color");
                        std::u32string bitmapAttr = element->GetAttribute(U"bitmap");
                        if (colorAttr.empty() && bitmapAttr.empty())
                        {
                            throw std::runtime_error("item '" + itemName + "' element " + std::to_string(i) + " has no 'color' and no 'bitmap' attribute");
                        }
                        if (!colorAttr.empty())
                        {
                            std::string colorHexString = ToUtf8(colorAttr);
                            SetColor(itemName, ParseColorHexString(colorHexString, itemName));
                        }
                        else if (!bitmapAttr.empty())
                        {
                            std::string bitmapName = ToUtf8(bitmapAttr);
                            SetBitmapName(itemName, bitmapName);
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, "Error loading theme '" + name + "' from  file " + filePath + ": " + ex.what());
    }
}

Color Theme::GetColor(const std::string& itemName) const
{
    auto it = itemColorMap.find(itemName);
    if (it != itemColorMap.cend())
    {
        return it->second;
    }
    else
    {
        return Color::Red;
    }
}

void Theme::SetColor(const std::string& itemName, const Color& color)
{
    itemColorMap[itemName] = color;
}

std::string Theme::GetBitmapName(const std::string& itemName) const
{
    auto it = itemBitmapNameMap.find(itemName);
    if (it != itemBitmapNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Theme::SetBitmapName(const std::string& itemName, const std::string& bitmapName)
{
    itemBitmapNameMap[itemName] = bitmapName;
}

class Themes
{
public:
    static void Init();
    static void Done();
    static Themes& Instance() { return *instance; }
    Theme* GetCurrentTheme() const { return currentTheme; }
    void SetCurrentTheme(const std::string& themeName);
    void AddTheme(Theme* theme);
    void LoadThemes();
    void SaveThemes();
    std::vector<std::string> GetThemeNames() const;
    void RegisterUpdateColorsListener(Control* control);
    void UnregisterUpdateColorsListener(Control* control);
private:
    static std::unique_ptr<Themes> instance;
    std::vector<std::unique_ptr<Theme>> themeVec;
    std::map<std::string, Theme*> themeMap;
    Theme* currentTheme;
    std::set<Control*> updateColorsListeners;
    Themes();
    void UpdateColors();
};

std::unique_ptr<Themes> Themes::instance;

void Themes::Init()
{
    instance.reset(new Themes());
}

void Themes::Done()
{
    instance.reset();
}

Themes::Themes() : currentTheme(nullptr)
{
}

void Themes::AddTheme(Theme* theme)
{
    themeVec.push_back(std::unique_ptr<Theme>(theme));
}

void Themes::UpdateColors()
{
    for (Control* control : updateColorsListeners)
    {
        control->UpdateColors();
    }
}

void Themes::RegisterUpdateColorsListener(Control* control)
{
    updateColorsListeners.insert(control);
}

void Themes::UnregisterUpdateColorsListener(Control* control)
{
    updateColorsListeners.erase(control);
}

std::vector<std::string> Themes::GetThemeNames() const
{
    std::vector<std::string> themeNames;
    for (const auto& theme : themeVec)
    {
        themeNames.push_back(theme->Name());
    }
    return themeNames;
}

void Themes::SaveThemes()
{
    sngxml::dom::Document themesDoc;
    sngxml::dom::Element* root = new sngxml::dom::Element(U"themes");
    for (const auto& theme : themeVec)
    {
        theme->Save();
        sngxml::dom::Element* themeElement = new sngxml::dom::Element(U"theme");
        themeElement->SetAttribute(U"name", ToUtf32(theme->Name()));
        themeElement->SetAttribute(U"filePath", ToUtf32(theme->FilePath()));
        root->AppendChild(std::unique_ptr<sngxml::dom::Node>(themeElement));
    }
    themesDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(root));
    std::string themesFilePath = GetThemesFilePath();
    std::ofstream file(themesFilePath);
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    themesDoc.Write(formatter);
}

void Themes::LoadThemes()
{
    themeVec.clear();
    std::string themesFilePath = GetThemesFilePath();
    std::unique_ptr<sngxml::dom::Document> themesDoc = sngxml::dom::ReadDocument(themesFilePath);
    std::unique_ptr<sngxml::xpath::XPathObject> themeObject = sngxml::xpath::Evaluate(U"/themes/theme", themesDoc.get());
    if (themeObject)
    {
        if (themeObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(themeObject.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string themeNameAttr = element->GetAttribute(U"name");
                    if (themeNameAttr.empty())
                    {
                        throw std::runtime_error("theme number " + std::to_string(i) + " has no 'name' attribute");
                    }
                    std::string themeName = ToUtf8(themeNameAttr);
                    std::u32string themeFilePathAttr = element->GetAttribute(U"filePath");
                    if (themeFilePathAttr.empty())
                    {
                        throw std::runtime_error("theme '" + themeName + " number " + std::to_string(i) + " has no 'filePath' attribute");
                    }
                    std::string themeFilePath = ToUtf8(themeFilePathAttr);
                    std::unique_ptr<Theme> themePtr(new Theme(themeName, themeFilePath));
                    Theme* theme = themePtr.get();
                    theme->Load();
                    themeMap[theme->Name()] = theme;
                    themeVec.push_back(std::move(themePtr));
                }
            }
        }
    }
}

void Themes::SetCurrentTheme(const std::string& themeName)
{
    if (currentTheme && currentTheme->Name() == themeName) return;
    auto it = themeMap.find(themeName);
    if (it != themeMap.cend())
    {
        currentTheme = it->second;
        UpdateColors();
    }
}

Color GetColor(const std::string& itemName)
{
    Theme* currentTheme = Themes::Instance().GetCurrentTheme();
    if (currentTheme)
    {
        return currentTheme->GetColor(itemName);
    }
    else
    {
        return Color::Red;
    }
}

std::string GetBitmapName(const std::string& itemName)
{
    Theme* currentTheme = Themes::Instance().GetCurrentTheme();
    if (currentTheme)
    {
        return currentTheme->GetBitmapName(itemName);
    }
    else
    {
        return std::string();
    }
}

std::string GetTheme()
{
    Theme* currentTheme = Themes::Instance().GetCurrentTheme();
    if (currentTheme)
    {
        return currentTheme->Name();
    }
    else
    {
        return std::string();
    }
}

void SetTheme(const std::string& themeName)
{
    Themes::Instance().SetCurrentTheme(themeName);
}

std::vector<std::string> GetThemes()
{
    return Themes::Instance().GetThemeNames();
}

void AddTheme(Theme* theme)
{
    Themes::Instance().AddTheme(theme);
}

bool ThemesFileExists()
{
    return boost::filesystem::exists(GetThemesFilePath());
}

void SaveThemes()
{
    Themes::Instance().SaveThemes();
}

void LoadThemes()
{
    Themes::Instance().LoadThemes();
}

void RegisterUpdateColorsListener(Control* control)
{
    Themes::Instance().RegisterUpdateColorsListener(control);
}

void UnregisterUpdateColorsListener(Control* control)
{
    Themes::Instance().UnregisterUpdateColorsListener(control);
}

void ThemeInit()
{
    Themes::Init();
}

void ThemeDone()
{
    Themes::Done();
}

} } // namespace cmajor::wing
