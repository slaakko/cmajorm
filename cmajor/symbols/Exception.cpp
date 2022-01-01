// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>

namespace cmajor { namespace symbols {

using namespace soulng::util;
using namespace soulng::unicode;

std::string Expand(const std::string& errorMessage, const Span& span, const boost::uuids::uuid& moduleId)
{
    std::vector<std::pair<Span, boost::uuids::uuid>> references;
    return Expand(errorMessage, span, moduleId, references);
}

std::string Expand(const std::string& errorMessage, const Span& primarySpan, const boost::uuids::uuid& primaryModuleId, 
    const Span& referenceSpan, const boost::uuids::uuid& referenceModuleId)
{
    std::vector<std::pair<Span, boost::uuids::uuid>> references(1, std::make_pair(referenceSpan, referenceModuleId));
    return Expand(errorMessage, primarySpan, primaryModuleId, references, "Error");
}

std::string Expand(const std::string& errorMessage, const Span& primarySpan, const boost::uuids::uuid& primaryModuleId, 
    const Span& referenceSpan, const boost::uuids::uuid& referenceModuleId, const std::string& title)
{
    std::vector<std::pair<Span, boost::uuids::uuid>> references(1, std::make_pair(referenceSpan, referenceModuleId));
    return Expand(errorMessage, primarySpan, primaryModuleId, references, title);
}

std::string Expand(const std::string& errorMessage, const Span& span, const boost::uuids::uuid& moduleId, const std::vector<std::pair<Span, boost::uuids::uuid>>& references)
{
    return Expand(errorMessage, span, moduleId, references, "Error");
}

std::string Expand(const std::string& errorMessage, const Span& span, const boost::uuids::uuid& moduleId, const std::vector<std::pair<Span, boost::uuids::uuid>>& references, 
    const std::string& title)
{
    std::vector<std::pair<Span, boost::uuids::uuid>> referenceSpans = references;
    referenceSpans.erase(std::unique(referenceSpans.begin(), referenceSpans.end()), referenceSpans.end());
    std::string expandedMessage = title + ": " + errorMessage;
    if (span.Valid())
    {
        Module* module = GetModuleById(moduleId);
        if (module)
        {
            std::string fileName = module->GetFilePath(span.fileIndex);
            if (!fileName.empty())
            {
                expandedMessage.append(" (file '" + fileName + "', line " + std::to_string(span.line) + ")");
                expandedMessage.append(":\n").append(ToUtf8(module->GetErrorLines(span)));
            }
        }
    }
    for (const std::pair<Span, boost::uuids::uuid>& referenceSpan : referenceSpans)
    {
        if (!referenceSpan.first.Valid()) continue;
        if (referenceSpan.first == span && referenceSpan.second == moduleId) continue;
        Module* module = GetModuleById(referenceSpan.second);
        if (module)
        {
            std::string fileName = module->GetFilePath(referenceSpan.first.fileIndex);
            if (!fileName.empty())
            {
                expandedMessage.append("\nsee reference to file '" + fileName + "', line " + std::to_string(referenceSpan.first.line));
                expandedMessage.append(":\n").append(ToUtf8(module->GetErrorLines(referenceSpan.first)));
            }
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

Exception::Exception(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_) : 
    what(Expand(message_, defined_, definedModuleId_)), message(message_), defined(defined_), definedModuleId(definedModuleId_)
{
}

Exception::Exception(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const Span& referenced_, const boost::uuids::uuid& referencedModuleId_) :
    what(Expand(message_, defined_, definedModuleId_, referenced_, referencedModuleId_)), message(message_), defined(defined_), definedModuleId(definedModuleId_)
{
    references.push_back(std::make_pair(referenced_, referencedModuleId_));
}

Exception::Exception(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, const std::vector<std::pair<Span, boost::uuids::uuid>>& references_) :
    what(Expand(message_, defined_, definedModuleId_, references_)), message(message_), defined(defined_), definedModuleId(definedModuleId_), references(references_)
{
}

Exception::~Exception()
{
}

std::unique_ptr<JsonValue> Exception::ToJson() const
{
    Module* module = cmajor::symbols::GetModuleById(definedModuleId);
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
    std::vector<std::pair<Span, boost::uuids::uuid>> referenceSpans = references;
    referenceSpans.erase(std::unique(referenceSpans.begin(), referenceSpans.end()), referenceSpans.end());
    for (const std::pair<Span, boost::uuids::uuid>& referenceSpanModuleId : referenceSpans)
    {
        if (!referenceSpanModuleId.first.Valid()) continue;
        if (referenceSpanModuleId.first == defined && referenceSpanModuleId.second == definedModuleId) continue;
        Module* mod = cmajor::symbols::GetModuleById(referenceSpanModuleId.second);
        std::unique_ptr<JsonObject> ref = SpanToJson(mod, referenceSpanModuleId.first);
        if (ref)
        {
            refs->AddItem(std::move(ref));
        }
    }
    json->AddField(U"references", std::move(refs));
    return std::unique_ptr<JsonValue>(json.release());
    return std::unique_ptr<JsonValue>();
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
    Module* module = cmajor::symbols::GetModuleById(definedModuleId);
    std::unique_ptr<sngxml::dom::Element> spanElement = SpanToDomElement(module, defined);
    if (spanElement)
    {
        diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(spanElement.release()));
    }
    diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
    for (const std::pair<Span, boost::uuids::uuid>& spanModuleId : references)
    {
        if (!spanModuleId.first.Valid()) continue;
        Module* mod = cmajor::symbols::GetModuleById(spanModuleId.second);
        std::unique_ptr<sngxml::dom::Element> diagnosticElement(new sngxml::dom::Element(U"diagnostic"));
        std::unique_ptr<sngxml::dom::Element> categoryElement(new sngxml::dom::Element(U"category"));
        std::unique_ptr<sngxml::dom::Text> categoryText(new sngxml::dom::Text(U"info"));
        categoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryText.release()));
        std::unique_ptr<sngxml::dom::Element> messageElement(new sngxml::dom::Element(U"message"));
        std::unique_ptr<sngxml::dom::Text> messageText(new sngxml::dom::Text(ToUtf32("see reference to")));
        messageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageText.release()));
        diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryElement.release()));
        diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement.release()));
        std::unique_ptr<sngxml::dom::Element> spanElement = SpanToDomElement(mod, spanModuleId.first);
        if (spanElement)
        {
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(spanElement.release()));
            diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
        }
    }
}

ModuleImmutableException::ModuleImmutableException(Module* module_, Module* immutableModule, const Span& defined_, const Span& referenced_) :
    Exception("attempt to add a symbol to an immutable module' " + ToUtf8(immutableModule->Name()) + "'", defined_, module_->Id(), referenced_, immutableModule->Id())
{
}

SymbolCheckException::SymbolCheckException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& moduleId_) : Exception(message_, defined_, moduleId_)
{
}

CastOverloadException::CastOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_) : Exception(message_, defined_, definedModuleId_)
{
}

CastOverloadException::CastOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
    const Span& referenced_, const boost::uuids::uuid& referencedModuleId_) : 
    Exception(message_, defined_, definedModuleId_, referenced_, referencedModuleId_)
{
}

CastOverloadException::CastOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
    const std::vector<std::pair<Span, boost::uuids::uuid>>& references_) : Exception(message_, defined_, definedModuleId_, references_)
{
}

CannotBindConstToNonconstOverloadException::CannotBindConstToNonconstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_) : 
    Exception(message_, defined_, definedModuleId_)
{
}

CannotBindConstToNonconstOverloadException::CannotBindConstToNonconstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
    const Span& referenced_, const boost::uuids::uuid& referencedModuleId_) :
    Exception(message_, defined_, definedModuleId_, referenced_, referencedModuleId_)
{
}

CannotBindConstToNonconstOverloadException::CannotBindConstToNonconstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_,
    const std::vector<std::pair<Span, boost::uuids::uuid>>& references_) :
    Exception(message_, defined_, definedModuleId_, references_)
{
}

CannotAssignToConstOverloadException::CannotAssignToConstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_) : 
    Exception(message_, defined_, definedModuleId_)
{
}

CannotAssignToConstOverloadException::CannotAssignToConstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
    const Span& referenced_, const boost::uuids::uuid& referencedModuleId_) :
    Exception(message_, defined_, definedModuleId_, referenced_, referencedModuleId_)
{
}

CannotAssignToConstOverloadException::CannotAssignToConstOverloadException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
    const std::vector<std::pair<Span, boost::uuids::uuid>>& references_) :
    Exception(message_, defined_, definedModuleId_, references_)
{
}

NoViableFunctionException::NoViableFunctionException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_) : 
    Exception(message_, defined_, definedModuleId_)
{
}

NoViableFunctionException::NoViableFunctionException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
    const Span& referenced_, const boost::uuids::uuid& referencedModuleId_) :
    Exception(message_, defined_, definedModuleId_, referenced_, referencedModuleId_)
{
}

NoViableFunctionException::NoViableFunctionException(const std::string& message_, const Span& defined_, const boost::uuids::uuid& definedModuleId_, 
    const std::vector<std::pair<Span, boost::uuids::uuid>>& references_) :
    Exception(message_, defined_, definedModuleId_, references_)
{
}

} } // namespace cmajor::symbols
