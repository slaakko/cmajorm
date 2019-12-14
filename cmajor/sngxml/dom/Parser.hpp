// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_DOM_PARSER_INCLUDED
#define SNGXML_DOM_PARSER_INCLUDED
#include <sngxml/dom/Document.hpp>

namespace sngxml { namespace dom {

enum class Flags : int
{
    none = 0, debug = 1 << 0
};

inline Flags operator&(Flags flags, Flags flag)
{
    return Flags(int(flags) & int(flag));
}

inline Flags operator|(Flags flags, Flags flag)
{
    return Flags(int(flags) | int(flag));
}

inline Flags operator~(Flags flags)
{
    return Flags(~int(flags));
}

SNGXML_DOM_API std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId);
SNGXML_DOM_API std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId, Flags flags);
SNGXML_DOM_API std::unique_ptr<Document> ReadDocument(const std::string& fileName);
SNGXML_DOM_API std::unique_ptr<Document> ReadDocument(const std::string& fileName, Flags flags);

} } // namespace sngxml::dom

#endif // SNGXML_DOM_PARSER_INCLUDED
