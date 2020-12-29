// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XMLSER_GEN_INCLUDED
#define SNGXML_XMLSER_GEN_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>
#include <string>

namespace sngxml { namespace xmlser {

SNGXML_SERIALIZATION_API void GenerateXmlSerializationCode(const std::string& serializationDefinitionFilePath, bool verbose);

} } // namespace sngxml::xmlser

#endif // SNGXML_XMLSER_GEN_INCLUDED
