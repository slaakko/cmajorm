// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_EXCEPTION_INCLUDED
#define CMAJOR_SYMBOLS_EXCEPTION_INCLUDED
#include <cmajor/parsing/Scanner.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/util/Json.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::util;
using cmajor::parsing::Span;

class Module;

std::string Expand(Module* module, const std::string& errorMessage, const Span& span);
std::string Expand(Module* module, const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan);
std::string Expand(Module* module, const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan, const std::string& title);
std::string Expand(Module* module, const std::string& errorMessage, const Span& span, const std::vector<Span>& references);
std::string Expand(Module* module, const std::string& errorMessage, const Span& span, const std::vector<Span>& references, const std::string& title);

std::unique_ptr<JsonObject> SpanToJson(Module* module, const Span& span);
std::unique_ptr<cmajor::dom::Element> SpanToDomElement(Module* module, const Span& span);

class Exception
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
    void AddToDiagnosticsElement(cmajor::dom::Element* diagnosticsElement) const;
private:
    Module* module;
    std::string what;
    std::string message;
    Span defined;
    std::vector<Span> references;
};

class ModuleImmutableException : public Exception
{
public:
    ModuleImmutableException(Module* module_, Module* immutableModule, const Span& defined_, const Span& referenced_);
};

class SymbolCheckException : public Exception
{
public:
    SymbolCheckException(Module* module_, const std::string& message_, const Span& defined_);
};

class CastOverloadException : public Exception
{
public:
    CastOverloadException(Module* module, const std::string& message_, const Span& defined_);
    CastOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    CastOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

class CannotBindConstToNonconstOverloadException : public Exception
{
public:
    CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_);
    CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

class CannotAssignToConstOverloadException : public Exception
{
public:
    CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_);
    CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

class NoViableFunctionException : public Exception
{
public:
    NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_);
    NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_);
    NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_);
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_EXCEPTION_INCLUDED
