// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_WARNING_INCLUDED
#define CMAJOR_SYMBOLS_WARNING_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <cmajor/parsing/Scanner.hpp>
#include <cmajor/util/Json.hpp>

namespace cmajor { namespace symbols {

using cmajor::parsing::Span;
using cmajor::util::JsonValue;
class Module;

class SYMBOLS_API Warning
{
public:
    Warning(const std::u32string& project_, const std::string& message_);
    const std::u32string& Project() const { return project; }
    const std::string& Message() const { return message; }
    const Span& Defined() const { return defined; }
    void SetDefined(const Span& defined_) { defined = defined_; }
    const std::vector<Span>& References() const { return references; }
    void SetReferences(const std::vector<Span>& references_);
    std::unique_ptr<JsonValue> ToJson(Module* module) const;
    Module* GetModule() { return module; }
    void SetModule(Module* module_) { module = module_; }
private:
    std::u32string project;
    std::string message;
    Span defined;
    std::vector<Span> references;
    Module* module;
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
