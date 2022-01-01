// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_DOM_PARSER_INCLUDED
#define SNGXML_DOM_PARSER_INCLUDED
#include <sngxml/dom/Document.hpp>
#include <soulng/util/SocketFwd.hpp>

namespace sngxml { namespace dom {

enum class Flags : int
{
    none = 0, debug = 1 << 0
};

inline Flags operator&(Flags flags, Flags flag)
{
    return static_cast<Flags>(static_cast<int>(flags) & static_cast<int>(flag));
}

inline Flags operator|(Flags flags, Flags flag)
{
    return static_cast<Flags>(static_cast<int>(flags) | static_cast<int>(flag));
}

inline Flags operator~(Flags flags)
{
    return static_cast<Flags>(~static_cast<int>(flags));
}

SNGXML_DOM_API std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId);
SNGXML_DOM_API std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId, Flags flags);
SNGXML_DOM_API std::unique_ptr<Document> ReadDocument(const std::string& fileName);
SNGXML_DOM_API std::unique_ptr<Document> ReadDocument(const std::string& fileName, Flags flags);
SNGXML_DOM_API void SendDocument(soulng::util::TcpSocket& socket, Document& document);
SNGXML_DOM_API std::unique_ptr<Document> ReceiveDocument(soulng::util::TcpSocket& socket);

} } // namespace sngxml::dom

#endif // SNGXML_DOM_PARSER_INCLUDED
