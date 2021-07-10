// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/KeywordSymbol.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

KeywordSymbol::KeywordSymbol(int tokenId_, const std::u32string& name_) : Symbol(SymbolType::keywordSymbol, Span(), boost::uuids::uuid(), name_), tokenId(tokenId_)
{
}

class KeywordSymbols
{
public:
    static void Init();
    static void Done();
    static KeywordSymbols& Instance() { return *instance; }
    KeywordSymbol* GetKeywordSymbol(int tokenId) const;
private:
    KeywordSymbols();
    static std::unique_ptr<KeywordSymbols> instance;
    std::vector<std::unique_ptr<KeywordSymbol>> symbols;
    std::map<int, KeywordSymbol*> symbolMap;
};

std::unique_ptr<KeywordSymbols> KeywordSymbols::instance;

void KeywordSymbols::Init()
{
    instance.reset(new KeywordSymbols());
}

void KeywordSymbols::Done()
{
    instance.reset();
}

KeywordSymbols::KeywordSymbols()
{
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(ASSERT, U"assert")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(ELIF, U"elif")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(ENDIF, U"endif")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(BOOL, U"bool")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(TRUE, U"true")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(FALSE, U"false")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(SBYTE, U"sbyte")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(BYTE, U"byte")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(SHORT, U"short")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(USHORT, U"ushort")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(INT, U"int")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(UINT, U"uint")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(LONG, U"long")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(ULONG, U"ulong")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(FLOAT, U"float")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(DOUBLE, U"double")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CHAR, U"char")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(WCHAR, U"wchar")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(UCHAR, U"uchar")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(VOID, U"void")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(ENUM, U"enum")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CAST, U"cast")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(INTERFACE, U"interface")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(NAMESPACE, U"namespace")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(USING, U"using")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(STATIC, U"static")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(EXTERN, U"extern")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(AS, U"as")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(IS, U"is")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(EXPLICIT, U"explicit")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(DELEGATE, U"delegate")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(INLINE, U"inline")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CDECL, U"cdecl")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(NOTHROW, U"nothrow")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(PUBLIC, U"public")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(PROTECTED, U"protected")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(PRIVATE, U"private")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(INTERNAL, U"internal")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(VIRTUAL, U"virtual")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(ABSTRACT, U"abstract")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(OVERRIDE, U"override")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(SUPPRESS, U"suppress")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(WINAPI, U"winapi")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(OPERATOR, U"operator")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CLASS, U"class")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(RETURN, U"return")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(IF, U"if")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(ELSE, U"else")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(SWITCH, U"switch")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CASE, U"case")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(DEFAULT, U"default")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(WHILE, U"while")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(DO, U"do")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(FOR, U"for")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(BREAK, U"break")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CONTINUE, U"continue")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(GOTO, U"goto")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(TYPEDEF, U"typedef")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(TYPENAME, U"typename")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(TYPEID, U"typeid")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CONST, U"const")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CONSTEXPR, U"constexpr")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(NULLLIT, U"null")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(THIS, U"this")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(BASE, U"base")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CONSTRUCT, U"construct")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(DESTROY, U"destroy")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(NEW, U"new")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(DELETE, U"delete")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(SIZEOF, U"sizeof")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(TRY, U"try")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CATCH, U"catch")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(THROW, U"throw")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(CONCEPT, U"concept")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(WHERE, U"where")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(AXIOM, U"axiom")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(AND, U"and")));
    symbols.push_back(std::unique_ptr<KeywordSymbol>(new KeywordSymbol(OR, U"or")));
    for (auto& symbol : symbols)
    {
        symbolMap[symbol->TokenId()] = symbol.get();
    }
}

KeywordSymbol* KeywordSymbols::GetKeywordSymbol(int tokenId) const
{
    auto it = symbolMap.find(tokenId);
    if (it != symbolMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

KeywordSymbol* GetKeywordSymbol(int tokenId)
{
    return KeywordSymbols::Instance().GetKeywordSymbol(tokenId);
}

void InitKeywordSymbol()
{
    KeywordSymbols::Init();
}

void DoneKeywordSymbol()
{
    KeywordSymbols::Done();
}

} } // namespace cmajor::symbols
