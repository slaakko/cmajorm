// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Warning.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::util;
using namespace soulng::unicode;

Warning::Warning(const std::u32string& project_, const std::string& message_) : project(project_), message(message_), module(nullptr)
{
}

void Warning::SetReferences(const std::vector<Span>& references_)
{
    references = references_;
}

std::unique_ptr<JsonValue> Warning::ToJson(Module* module) const
{
    JsonObject* o = new JsonObject();
    o->AddField(U"project", std::unique_ptr<JsonValue>(new JsonString(project)));
    o->AddField(U"message", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(message))));
    JsonArray* referencesArray = new JsonArray();
    std::unique_ptr<JsonValue> def = SpanToJson(module, defined);
    if (def)
    {
        referencesArray->AddItem(std::move(def));
    }
    for (const Span& span : references)
    {
        std::unique_ptr<JsonValue> ref = SpanToJson(module, span);
        if (ref)
        {
            referencesArray->AddItem(std::move(ref));
        }
    }
    o->AddField(U"references", std::unique_ptr<JsonValue>(referencesArray));
    return std::unique_ptr<JsonValue>(o);
}

CompileWarningCollection::CompileWarningCollection()
{
}

void CompileWarningCollection::AddWarning(const Warning& warning)
{
    warnings.push_back(warning);
}

} } // namespace cmajor::symbols
