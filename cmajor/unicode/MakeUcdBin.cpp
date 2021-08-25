#include <sngxml/xml/XmlParserInterface.hpp>
#include <soulng/lexer/SourcePos.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <stdint.h>
#include <set>
#include <map>

using namespace soulng::unicode;

using soulng::lexer::SourcePos;

class UnicodeCharacterDatabaseContentHandler : public sngxml::xml::XmlContentHandler
{
public:
    UnicodeCharacterDatabaseContentHandler();
    void StartElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName, const sngxml::xml::Attributes& attributes, const SourcePos& sourcePos) override;
    void EndElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName) override;
private:
    char32_t codePoint;
    ExtendedCharacterInfo* extendedCharacterInfo;
};

UnicodeCharacterDatabaseContentHandler::UnicodeCharacterDatabaseContentHandler() : codePoint(-1), extendedCharacterInfo(nullptr)
{
}

std::set<std::u32string> elementNames;
std::set<std::u32string> attributeNames;
std::set<std::u32string> booleanAttributeNames;
std::set<std::string> unseenPropertyNames;
std::vector<std::unique_ptr<Block>> blocks;
std::map<std::string, Block*>  blockMap;
std::ofstream iso88591;

uint32_t FromHex(const std::string& hex)
{
    uint32_t c = 0;
    std::stringstream s;
    s.str(hex);
    s >> std::hex >> c;
    return c;
}

void UnicodeCharacterDatabaseContentHandler::StartElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName, 
    const sngxml::xml::Attributes& attributes, const SourcePos& sourcePos)
{
    auto it = elementNames.find(qualifiedName);
    if (it == elementNames.cend())
    {
        elementNames.insert(qualifiedName);
    }
    if (qualifiedName == U"char")
    {
        const std::u32string* cp = attributes.GetAttributeValue(U"cp");
        CharacterInfo* characterInfo = nullptr;
        extendedCharacterInfo = nullptr;
        uint32_t c = 0;
        if (cp)
        {
            std::string hex = ToUtf8(*cp);
            c = FromHex(hex);
            codePoint = c;
            characterInfo = &CreateCharacterInfo(c);
            extendedCharacterInfo = &CreateExtendedCharacterInfo(c);
        }
        for (const sngxml::xml::Attribute& attribute : attributes)
        {
            auto it = attributeNames.find(attribute.QualifiedName());
            if (it == attributeNames.cend())
            {
                attributeNames.insert(attribute.QualifiedName());
            }
            if (cp && characterInfo && extendedCharacterInfo)
            {
                std::string propertyName = ToUtf8(attribute.QualifiedName());
                std::string attributeValue = ToUtf8(attribute.Value());
                if (BinaryPropertyTable::Instance().IsBinaryProperty(propertyName))
                {
                    const BinaryProperty& property = BinaryPropertyTable::Instance().GetBinaryPropertyByShortName(propertyName);
                    BinaryPropertyId id = property.Id();
                    bool value = false;
                    if (attributeValue == "Y")
                    {
                        value = true;
                    }
                    else if (attributeValue != "N")
                    {
                        throw std::runtime_error("binary property value not Y/N: at line " + std::to_string(sourcePos.line) + ", column " + std::to_string(sourcePos.col));
                    }
                    characterInfo->SetBinaryPropery(id, value);
                }
                else if (propertyName == "age")
                {
                    const Age& age = AgeTable::Instance().GetAge(attributeValue);
                    characterInfo->SetAge(age.Id());
                }
                else if (propertyName == "gc")
                {
                    const GeneralCategory& generalCategory = GeneralCategoryTable::Instance().GetGeneralCategoryByShortName(attributeValue);
                    characterInfo->SetGeneralCategory(generalCategory.Id());
                }
                else if (propertyName == "blk")
                {
                    const Block& block = BlockTable::Instance().GetBlockByShortName(attributeValue);
                    characterInfo->SetBlock(block.Id());
                }
                else if (propertyName == "suc")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetUpper(c);
                    }
                    else
                    {
                        uint32_t upper = FromHex(attributeValue);
                        characterInfo->SetUpper(upper);
                    }
                }
                else if (propertyName == "slc")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetLower(c);
                    }
                    else
                    {
                        uint32_t lower = FromHex(attributeValue);
                        characterInfo->SetLower(lower);
                    }
                }
                else if (propertyName == "stc")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetTitle(c);
                    }
                    else
                    {
                        uint32_t title = FromHex(attributeValue);
                        characterInfo->SetTitle(title);
                    }
                }
                else if (propertyName == "scf")
                {
                    if (attributeValue == "#")
                    {
                        characterInfo->SetFolding(c);
                    }
                    else
                    {
                        uint32_t folding = FromHex(attributeValue);
                        characterInfo->SetFolding(folding);
                    }
                }
                else if (propertyName == "sc")
                {
                    ScriptId script = GetScriptByShortName(attributeValue).Id();
                    characterInfo->SetScript(script);
                }
                else if (propertyName == "bc")
                {
                    BidiClassId bidiClass = GetBidiClassByShortName(attributeValue).Id();
                    extendedCharacterInfo->SetBidiClass(bidiClass);
                }
                else if (propertyName == "uc")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullUpper().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullUpper().append(1, u);
                        }
                    }
                }
                else if (propertyName == "lc")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullLower().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullLower().append(1, u);
                        }
                    }
                }
                else if (propertyName == "tc")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullTitle().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullTitle().append(1, u);
                        }
                    }
                }
                else if (propertyName == "cf")
                {
                    if (attributeValue == "#")
                    {
                        extendedCharacterInfo->FullFolding().append(1, c);
                    }
                    else
                    {
                        std::vector<std::string> v = Split(attributeValue, ' ');
                        for (const std::string& s : v)
                        {
                            uint32_t u = FromHex(s);
                            extendedCharacterInfo->FullFolding().append(1, u);
                        }
                    }
                }
                else if (propertyName == "ccc")
                {
                    int value = boost::lexical_cast<int>(attributeValue);
                    if (value < 0 || value > 254)
                    {
                        throw std::runtime_error("invalid ccc attribute value");
                    }
                    extendedCharacterInfo->SetCanonicalCombiningClass(static_cast<uint8_t>(value));
                }
                else if (propertyName == "na")
                {
                    extendedCharacterInfo->SetCharacterName(attributeValue);
                }
                else if (propertyName == "na1")
                {
                    extendedCharacterInfo->SetUnicode1Name(attributeValue);
                }
                else if (propertyName == "nv")
                {
                    if (attributeValue != "NaN")
                    {
                        extendedCharacterInfo->SetNumericValue(attributeValue);
                    }
                }
                else if (propertyName == "nt")
                {
                    extendedCharacterInfo->SetNumericType(GetNumericTypeByShortName(attributeValue).Id());
                }
                else if (propertyName == "bmg")
                {
                    uint32_t bmg = FromHex(attributeValue);
                    extendedCharacterInfo->SetBidiMirroringGlyph(bmg);
                }
                else if (propertyName == "bpt")
                {
                    extendedCharacterInfo->SetBidiPairedBracketType(GetBidiPairedBracketTypeByShortName(attributeValue).Id());
                }
                else if (propertyName == "bpb")
                {
                    uint32_t bpb = FromHex(attributeValue);
                    extendedCharacterInfo->SetBidiPairedBracket(bpb);
                }
                else
                {
                    unseenPropertyNames.insert(propertyName);
                }
            }
        }
    }
    else if (qualifiedName == U"name-alias" && codePoint != -1 && extendedCharacterInfo != nullptr)
    {
        std::string alias;
        std::string type;
        for (const sngxml::xml::Attribute& attribute : attributes)
        {
            std::string attributeName = ToUtf8(attribute.QualifiedName());
            if (attributeName == "alias")
            {
                alias = ToUtf8(attribute.Value());
            }
            else if (attributeName == "type")
            {
                type = ToUtf8(attribute.Value());
            }
        }
        const AliasType& aliasType = AliasTypeTable::Instance().GetAliasType(type);
        extendedCharacterInfo->Aliases().push_back(Alias(aliasType.Id(), alias));
    }
}

void UnicodeCharacterDatabaseContentHandler::EndElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName)
{
    if (qualifiedName == U"char")
    {
        codePoint = -1;
        extendedCharacterInfo = nullptr;
    }
}

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
    }
    ~InitDone()
    {
        soulng::util::Done();
    }
};

int main(int argc, const char** argv)
{
    try
    {
        InitDone initDone;
        std::string cmajor_root;
        const char* cmajor_root_env = getenv("CMAJOR_ROOT");
        if (cmajor_root_env)
        {
            cmajor_root = cmajor_root_env;
        }
        if (cmajor_root.empty())
        {
            std::cerr << "please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory." << std::endl;
            return 2;
        }
        std::string xmlFileName = (boost::filesystem::path(cmajor_root) / boost::filesystem::path("unicode") / boost::filesystem::path("ucd.all.flat.xml")).generic_string();
        std::cout << "processing " << xmlFileName << "...";
        UnicodeCharacterDatabaseContentHandler contentHandler;
        sngxml::xml::ParseXmlFile(xmlFileName, &contentHandler);
        CharacterTable::Instance().Write();
        std::cout << "\b\b\b, done." << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << std::endl;
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
