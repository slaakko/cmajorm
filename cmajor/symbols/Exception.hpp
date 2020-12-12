// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_EXCEPTION_INCLUDED
#define CMAJOR_SYMBOLS_EXCEPTION_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/lexer/Span.hpp>
#include <boost/uuid/uuid.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::util;
using soulng::lexer::Span;

class Module;

std::string Expand(const std::string& errorMessage, const Span& span, const boost::uuids::uuid& moduleId);
std::string Expand(const std::string& errorMessage, const Span& primarySpan, const boost::uuids::uuid& primaryModuleId, const Span& referenceSpan, const boost::uuids::uuid& referenceModuleId);
std::string Expand(const std::string& errorMessage, const Span& primarySpan, const boost::uuids::uuid& primaryModuleId, const Span& referenceSpan, const boost::uuids::uuid& referenceModuleId, 
    const std::string& title);
std::string Expand(const std::string& errorMessage, const Span& span, const boost::uuids::uuid& moduleId, const std::vector<std::pair<Span, boost::uuids::uuid>>& references);
std::string Expand(const std::string& errorMessage, const Span& span, const boost::uuids::uuid& moduleId, const std::vector<std::pair<Span, boost::uuids::uuid>>& references, 
    const std::string& title);

SYMBOLS_API std::unique_ptr<JsonObject> SpanToJson(Module* module, const Span& span);
SYMBOLS_API std::unique_ptr<sngxml::dom::Element> SpanToDomElement(Module* module, const Span& span);

class SYMBOLS_API Exception
{
public:
    Exception(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_);
    Exception(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const Span& referenced_, const boost::uuids::uuid& referencedModuleId_);
    Exception(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const std::vector<std::pair<Span, boost::uuids::uuid>>& references_);
    virtual ~Exception();
    const std::string& What() const { return what; }
    const std::string& Message() const { return message; }
    const Span& Defined() const { return defined; }
    const boost::uuids::uuid& DefinedModuleId() const { return definedModuleId; }
    const std::vector<std::pair<Span, boost::uuids::uuid>>& References() const { return references; }
    std::unique_ptr<JsonValue> ToJson() const;
    void AddToDiagnosticsElement(sngxml::dom::Element* diagnosticsElement) const;
private:
    std::string what;
    std::string message;
    Span defined;
    boost::uuids::uuid definedModuleId;
    std::vector<std::pair<Span, boost::uuids::uuid>> references;
};

class SYMBOLS_API ModuleImmutableException : public Exception
{
public:
    ModuleImmutableException(Module* module_, Module* immutableModule, const Span& defined_, const Span& referenced_);
};

class SYMBOLS_API SymbolCheckException : public Exception
{
public:
    SymbolCheckException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& moduleId_);
};

class SYMBOLS_API CastOverloadException : public Exception
{
public:
    CastOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_);
    CastOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const Span& referenced_, const boost::uuids::uuid& referencedModuleId_);
    CastOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const std::vector<std::pair<Span, boost::uuids::uuid>>& references_);
};

class SYMBOLS_API CannotBindConstToNonconstOverloadException : public Exception
{
public:
    CannotBindConstToNonconstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_);
    CannotBindConstToNonconstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
        const Span& referenced_, const boost::uuids::uuid& referencedModuleId_);
    CannotBindConstToNonconstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
        const std::vector<std::pair<Span, boost::uuids::uuid>>& references_);
};

class SYMBOLS_API CannotAssignToConstOverloadException : public Exception
{
public:
    CannotAssignToConstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_);
    CannotAssignToConstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
        const Span& referenced_, const boost::uuids::uuid& referencedModuleId_);
    CannotAssignToConstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
        const std::vector<std::pair<Span, boost::uuids::uuid>>& references_);
};

class SYMBOLS_API NoViableFunctionException : public Exception
{
public:
    NoViableFunctionException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_);
    NoViableFunctionException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const Span& referenced_, const boost::uuids::uuid& referencedModuleId_);
    NoViableFunctionException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const std::vector<std::pair<Span, boost::uuids::uuid>>& references_);
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_EXCEPTION_INCLUDED
