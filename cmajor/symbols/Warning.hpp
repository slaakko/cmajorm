// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_WARNING_INCLUDED
#define CMAJOR_SYMBOLS_WARNING_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/lexer/Span.hpp>
#include <boost/uuid/uuid.hpp>

namespace cmajor { namespace symbols {

using soulng::lexer::Span;
using soulng::util::JsonValue;
class Module;

class SYMBOLS_API Warning
{
public:
    Warning(const std::u32string& project_, const std::string& message_);
    const std::u32string& Project() const { return project; }
    const std::string& Message() const { return message; }
    const Span& Defined() const { return defined; }
    void SetDefined(const Span& defined_, const boost::uuids::uuid& definedModuleId_) { defined = defined_; definedModuleId = definedModuleId_; }
    const std::vector<std::pair<Span, boost::uuids::uuid>>& References() const { return references; }
    void SetReferences(const std::vector<std::pair<Span, boost::uuids::uuid>>& references_);
    //std::unique_ptr<JsonValue> ToJson() const;
private:
    std::u32string project;
    std::string message;
    Span defined;
    boost::uuids::uuid definedModuleId;
    std::vector<std::pair<Span, boost::uuids::uuid>> references;
};

class SYMBOLS_API CompileWarningCollection
{
public:
    CompileWarningCollection();
    void AddWarning(const Warning& warning);
    const std::vector<Warning>& Warnings() const { return warnings; }
private:
    std::vector<Warning> warnings;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_WARNING_INCLUDED
