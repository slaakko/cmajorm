// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>

namespace cmajor { namespace symbols {

using namespace soulng::util;
using namespace soulng::unicode;

std::string Expand(Module* module, const std::string& errorMessage, const Span& span)
{
    std::vector<Span> references;
    return Expand(module, errorMessage, span, references);
}

std::string Expand(Module* module, const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan)
{
    std::vector<Span> references(1, referenceSpan);
    return Expand(module, errorMessage, primarySpan, references, "Error");
}

std::string Expand(Module* module, const std::string& errorMessage, const Span& primarySpan, const Span& referenceSpan, const std::string& title)
{
    std::vector<Span> references(1, referenceSpan);
    return Expand(module, errorMessage, primarySpan, references, title);
}

std::string Expand(Module* module, const std::string& errorMessage, const Span& span, const std::vector<Span>& references)
{
    return Expand(module, errorMessage, span, references, "Error");
}

std::string Expand(Module* module, const std::string& errorMessage, const Span& span, const std::vector<Span>& references, const std::string& title)
{
    std::vector<Span> referenceSpans = references;
    referenceSpans.erase(std::unique(referenceSpans.begin(), referenceSpans.end()), referenceSpans.end());
    std::string expandedMessage = title + ": " + errorMessage;
    if (span.Valid())
    {
        if (!module)
        {
            throw std::runtime_error("module not set");
        }
        std::string fileName = module->GetFilePath(span.fileIndex);
        if (!fileName.empty())
        {
            expandedMessage.append(" (file '" + fileName + "', line " + std::to_string(span.line) + ")");
            expandedMessage.append(":\n").append(ToUtf8(module->GetErrorLines(span)));
        }
    }
    for (const Span& referenceSpan : referenceSpans)
    {
        if (!referenceSpan.Valid()) continue;
        if (referenceSpan == span) continue;
        if (!module)
        {
            throw std::runtime_error("module not set");
        }
        std::string fileName = module->GetFilePath(referenceSpan.fileIndex);
        if (!fileName.empty())
        {
            expandedMessage.append("\nsee reference to file '" + fileName + "', line " + std::to_string(referenceSpan.line));
            expandedMessage.append(":\n").append(ToUtf8(module->GetErrorLines(referenceSpan)));
        }
    }
    return expandedMessage;
}

std::unique_ptr<JsonObject> SpanToJson(Module* module, const Span& span)
{
    if (!span.Valid()) return std::unique_ptr<JsonObject>();
    if (!module)
    {
        throw std::runtime_error("module not set");
    }
    const std::string& fileName = module->GetFilePath(span.fileIndex);
    if (fileName.empty()) return std::unique_ptr<JsonObject>();
    std::unique_ptr<JsonObject> json(new JsonObject());
    json->AddField(U"file", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(fileName))));
    json->AddField(U"line", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(span.line)))));
    std::u32string text = module->GetErrorLines(span);
    int32_t startCol = 0;
    int32_t endCol = 0;
    module->GetColumns(span, startCol, endCol); 
    json->AddField(U"startCol", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(startCol)))));
    json->AddField(U"endCol", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(endCol)))));
    json->AddField(U"text", std::unique_ptr<JsonValue>(new JsonString(text))); 
    return json;
}

std::unique_ptr<sngxml::dom::Element> SpanToDomElement(Module* module, const Span& span)
{
    if (!span.Valid()) return std::unique_ptr<sngxml::dom::Element>();
    if (!module)
    {
        throw std::runtime_error("module not set");
    }
    const std::string& fileName = module->GetFilePath(span.fileIndex);
    if (fileName.empty()) return std::unique_ptr<sngxml::dom::Element>();
    std::unique_ptr<sngxml::dom::Element> spanElement(new sngxml::dom::Element(U"span"));
    std::unique_ptr<sngxml::dom::Element> fileElement(new sngxml::dom::Element(U"file"));
    std::unique_ptr<sngxml::dom::Text> fileText(new sngxml::dom::Text(ToUtf32(fileName)));
    fileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(fileText.release()));
    std::unique_ptr<sngxml::dom::Element> lineElement(new sngxml::dom::Element(U"line"));
    std::unique_ptr<sngxml::dom::Text> lineText(new sngxml::dom::Text(ToUtf32(std::to_string(span.line))));
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lineText.release()));
    std::u32string text = module->GetErrorLines(span); 
    int32_t startCol = 0;
    int32_t endCol = 0;
    module->GetColumns(span, startCol, endCol);
    spanElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(fileElement.release()));
    spanElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lineElement.release()));
    std::unique_ptr<sngxml::dom::Element> startColElement(new sngxml::dom::Element(U"startCol"));
    std::unique_ptr<sngxml::dom::Text> startColText(new sngxml::dom::Text(ToUtf32(std::to_string(startCol))));
    startColElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(startColText.release()));
    spanElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(startColElement.release()));
    std::unique_ptr<sngxml::dom::Element> endColElement(new sngxml::dom::Element(U"endCol"));
    std::unique_ptr<sngxml::dom::Text> endColText(new sngxml::dom::Text(ToUtf32(std::to_string(endCol))));
    endColElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(endColText.release()));
    spanElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(endColElement.release()));
    std::unique_ptr<sngxml::dom::Element> textElement(new sngxml::dom::Element(U"text"));
    std::unique_ptr<sngxml::dom::Text> textText(new sngxml::dom::Text(text)); 
    textElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(textText.release())); 
    spanElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(textElement.release()));
    return spanElement;
}

Exception::Exception(Module* module_, const std::string& message_, const Span& defined_) : module(module_), what(Expand(module, message_, defined_)), message(message_), defined(defined_)
{
}

Exception::Exception(Module* module_, const std::string& message_, const Span& defined_, const Span& referenced_) : module(module_), what(Expand(module, message_, defined_, referenced_)), message(message_), defined(defined_)
{
    references.push_back(referenced_);
}

Exception::Exception(Module* module_, const std::string& message_, const Span& defined_, const std::vector<Span>& references_) :
    module(module_), what(Expand(module, message_, defined_, references_)), message(message_), defined(defined_), references(references_)
{
}

Exception::~Exception()
{
}

std::unique_ptr<JsonValue> Exception::ToJson() const
{
    std::unique_ptr<JsonObject> json(new JsonObject());
    json->AddField(U"tool", std::unique_ptr<JsonValue>(new JsonString(module->GetCurrentToolName())));
    json->AddField(U"kind", std::unique_ptr<JsonValue>(new JsonString(U"error")));
    json->AddField(U"project", std::unique_ptr<JsonValue>(new JsonString(module->GetCurrentProjectName())));
    json->AddField(U"message", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(message))));
    std::unique_ptr<JsonArray> refs(new JsonArray());
    std::unique_ptr<JsonObject> ref = SpanToJson(module, defined);
    if (ref)
    {
        refs->AddItem(std::move(ref));
    }
    std::vector<Span> referenceSpans = references;
    referenceSpans.erase(std::unique(referenceSpans.begin(), referenceSpans.end()), referenceSpans.end());
    for (const Span& referenceSpan : referenceSpans)
    {
        if (!referenceSpan.Valid()) continue;
        if (referenceSpan == defined) continue;
        std::unique_ptr<JsonObject> ref = SpanToJson(module, referenceSpan);
        if (ref)
        {
            refs->AddItem(std::move(ref));
        }
    }
    json->AddField(U"references", std::move(refs));
    return std::unique_ptr<JsonValue>(json.release());
}

void Exception::AddToDiagnosticsElement(sngxml::dom::Element* diagnosticsElement) const
{
    std::unique_ptr<sngxml::dom::Element> diagnosticElement(new sngxml::dom::Element(U"diagnostic"));
    std::unique_ptr<sngxml::dom::Element> categoryElement(new sngxml::dom::Element(U"category"));
    std::unique_ptr<sngxml::dom::Text> categoryText(new sngxml::dom::Text(U"error"));
    categoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryText.release()));
    std::unique_ptr<sngxml::dom::Element> subcategoryElement(new sngxml::dom::Element(U"subcategory"));
    std::unique_ptr<sngxml::dom::Text> subcategoryText(new sngxml::dom::Text(U"error"));
    subcategoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subcategoryText.release()));
    std::unique_ptr<sngxml::dom::Element> messageElement(new sngxml::dom::Element(U"message"));
    std::unique_ptr<sngxml::dom::Text> messageText(new sngxml::dom::Text(ToUtf32(message)));
    messageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageText.release()));
    diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryElement.release()));
    diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subcategoryElement.release()));
    diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement.release()));
    std::unique_ptr<sngxml::dom::Element> spanElement = SpanToDomElement(module, defined);
    if (spanElement)
    {
        diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(spanElement.release()));
    }
    diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
    for (const Span& span : references)
    {
        if (!span.Valid()) continue;
        std::unique_ptr<sngxml::dom::Element> diagnosticElement(new sngxml::dom::Element(U"diagnostic"));
        std::unique_ptr<sngxml::dom::Element> categoryElement(new sngxml::dom::Element(U"category"));
        std::unique_ptr<sngxml::dom::Text> categoryText(new sngxml::dom::Text(U"info"));
        categoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryText.release()));
        std::unique_ptr<sngxml::dom::Element> messageElement(new sngxml::dom::Element(U"message"));
        std::unique_ptr<sngxml::dom::Text> messageText(new sngxml::dom::Text(ToUtf32("see reference to")));
        messageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageText.release()));
        diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryElement.release()));
        diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement.release()));
        std::unique_ptr<sngxml::dom::Element> spanElement = SpanToDomElement(module, span);
        if (spanElement)
        {
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(spanElement.release()));
            diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
        }
    }

}

ModuleImmutableException::ModuleImmutableException(Module* module_, Module* immutableModule, const Span& defined_, const Span& referenced_) :
    Exception(module_, "attempt to add a symbol to an immutable module' " + ToUtf8(immutableModule->Name()) + "'", defined_, referenced_)
{
}

SymbolCheckException::SymbolCheckException(Module* module_, const std::string& message_, const Span& defined_) : Exception(module_, message_, defined_)
{
}

CastOverloadException::CastOverloadException(Module* module, const std::string& message_, const Span& defined_) : Exception(module, message_, defined_)
{
}

CastOverloadException::CastOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_) : Exception(module, message_, defined_, referenced_)
{
}

CastOverloadException::CastOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_) : Exception(module, message_, defined_, references_)
{
}

CannotBindConstToNonconstOverloadException::CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_) : Exception(module, message_, defined_)
{
}

CannotBindConstToNonconstOverloadException::CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_) :
    Exception(module, message_, defined_, referenced_)
{
}

CannotBindConstToNonconstOverloadException::CannotBindConstToNonconstOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_) :
    Exception(module, message_, defined_, references_)
{
}

CannotAssignToConstOverloadException::CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_) : Exception(module, message_, defined_)
{
}

CannotAssignToConstOverloadException::CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_) :
    Exception(module, message_, defined_, referenced_)
{
}

CannotAssignToConstOverloadException::CannotAssignToConstOverloadException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_) :
    Exception(module, message_, defined_, references_)
{
}

NoViableFunctionException::NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_) : Exception(module, message_, defined_)
{
}

NoViableFunctionException::NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_, const Span& referenced_) :
    Exception(module, message_, defined_, referenced_)
{
}

NoViableFunctionException::NoViableFunctionException(Module* module, const std::string& message_, const Span& defined_, const std::vector<Span>& references_) :
    Exception(module, message_, defined_, references_)
{
}

} } // namespace cmajor::symbols
