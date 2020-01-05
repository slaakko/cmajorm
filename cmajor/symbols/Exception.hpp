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

namespace cmajor { namespace symbols {

using namespace soulng::util;
using soulng::lexer::Span;

class Module;

std::string Expand(Module* module, const std::string& errorMessage, const Span& span);
std::string Expand(Module* module, const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan);
std::string Expand(Module* module, const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan, const std::string& title);
std::string Expand(Module* module, const std::string& errorMessage, const Span& span, const std::vector<Span>& references);
std::string Expand(Module* module, const std::string& errorMessage, const Span& span, const std::vector<Span>& references, const std::string& title);

SYMBOLS_API std::unique_ptr<JsonObject> SpanToJson(Module* module, const Span& span);
SYMBOLS_API std::unique_ptr<sngxml::dom::Element> SpanToDomElement(Module* module, const Span& span);

class SYMBOLS_API Exception
{
public:
    Exception(Module* module_, const std::string& message_, const Span& defined_);
    Exception(Module* module_, const std::string& message_, const Span& defined_, const Span& referenced_);
    Exception(Module* module_, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
    virtual ~Exception();
    Module* GetModule() const { return module; }
    const std::string& What() const { return what; }
    const std::string& Message() const { return message; }
    const Span& Defined() const { return defined; }
    const std::vector<Span>& References() const { return references; }
    std::unique_ptr<JsonValue> ToJson() const;
    void AddToDiagnosticsElement(sngxml::dom::Element* diagnosticsElement) const;
private:
    Module* module;
    std::string what;
    std::string message;
    Span defined;
    std::vector<Span> references;
};

class SYMBOLS_API ModuleImmutableException : public Exception
{
public:
    ModuleImmutableException(Module* module_, Module* immutableModule, const Span& defined_, const Span& referenced_);
};

class SYMBOLS_API SymbolCheckException : public Exception
{
public:
    SymbolCheckException(Module* module_, const std::string& message_, const Span& defined_);
};

class SYMBOLS_API CastOverloadException : public Exception
{
public:
    CastOverloadException(Module* module, const std::string& message_, const Span& defined_);
    CastOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    CastOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

class SYMBOLS_API CannotBindConstToNonconstOverloadException : public Exception
{
public:
    CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_);
    CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

class SYMBOLS_API CannotAssignToConstOverloadException : public Exception
{
public:
    CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_);
    CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

class SYMBOLS_API NoViableFunctionException : public Exception
{
public:
    NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_);
    NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_EXCEPTION_INCLUDED
