// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Metrics.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace cmajor { namespace wing {

using namespace soulng::util;
using namespace soulng::unicode;

std::string GetDefaultMetricsFilePath()
{
    std::string metricsFilePath;
    const char* cmajorRoot = getenv("CMAJOR_ROOT");
    if (cmajorRoot && *cmajorRoot)
    {
        metricsFilePath = Path::Combine(Path::Combine(cmajorRoot, "config"), "metrics.xml");
    }
    else
    {
        throw std::runtime_error("error: 'CMAJOR_ROOT' environment variable not set, please set it to contain /path/to/cmajor directory");
    }
    return metricsFilePath;
}

SizeElement::SizeElement(const std::string& name_) : name(name_)
{
}

void SizeElement::Read(sngxml::dom::Element* parentElement) 
{
    std::unique_ptr<sngxml::xpath::XPathObject> result = sngxml::xpath::Evaluate(ToUtf32(name), parentElement);
    if (result)
    {
        if (result->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(result.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    size.Width = boost::lexical_cast<float>(ToUtf8(element->GetAttribute(U"width")));
                    size.Height = boost::lexical_cast<float>(ToUtf8(element->GetAttribute(U"height")));
                }
            }
        }
    }
}

void SizeElement::Write(sngxml::dom::Element* parentElement)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(name)));
    element->SetAttribute(U"width", ToUtf32(std::to_string(size.Width)));
    element->SetAttribute(U"height", ToUtf32(std::to_string(size.Height)));
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(element.release()));
}

Metrics::Metrics() : 
    dpiX(96), dpiY(96), 
    defaultButtonSize(88, 25), defaultButtonSizeElement("button"), 
    defaultLabelSize(71, 19), defaultLabelSizeElement("label"),
    defaultTextBoxSize(94, 14), defaultTextBoxSizeElement("textBox"),
    defaultListBoxSize(120, 96), defaultListBoxSizeElement("listBox"),
    defaultCheckBoxSize(80, 17), defaultCheckBoxSizeElement("checkBox"),
    defaultControlSpacing(10, 10), defaultControlSpacingElement("controlSpacing")
{
    SetElementValues();
}

void Metrics::SetElementValues()
{
    defaultButtonSizeElement.Set(SizeF(HorizontalPixelsToMM(defaultButtonSize.Width), VerticalPixelsToMM(defaultButtonSize.Height)));
    defaultLabelSizeElement.Set(SizeF(HorizontalPixelsToMM(defaultLabelSize.Width), VerticalPixelsToMM(defaultLabelSize.Height)));
    defaultTextBoxSizeElement.Set(SizeF(HorizontalPixelsToMM(defaultTextBoxSize.Width), VerticalPixelsToMM(defaultTextBoxSize.Height)));
    defaultListBoxSizeElement.Set(SizeF(HorizontalPixelsToMM(defaultListBoxSize.Width), VerticalPixelsToMM(defaultListBoxSize.Height)));
    defaultCheckBoxSizeElement.Set(SizeF(HorizontalPixelsToMM(defaultCheckBoxSize.Width), VerticalPixelsToMM(defaultCheckBoxSize.Height)));
    defaultControlSpacingElement.Set(SizeF(HorizontalPixelsToMM(defaultControlSpacing.Width), VerticalPixelsToMM(defaultControlSpacing.Height)));
}

void Metrics::LoadFromFile(const std::string& fileName)
{
    if (boost::filesystem::exists(fileName))
    {
        std::unique_ptr<sngxml::dom::Document> metricsDoc = sngxml::dom::ReadDocument(fileName);
        Read(metricsDoc->DocumentElement());
    }
}

void Metrics::SaveToFile(const std::string& fileName, bool setElementValues)
{
    if (setElementValues)
    {
        SetElementValues();
    }
    sngxml::dom::Document metricsDoc;
    metricsDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"metrics")));
    metricsDoc.DocumentElement()->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Comment(U"metrics are in millimeters")));
    metricsDoc.DocumentElement()->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"\n")));
    Write(metricsDoc.DocumentElement());
    std::ofstream metricsFile(fileName);
    CodeFormatter formatter(metricsFile);
    formatter.SetIndentSize(1);
    metricsDoc.Write(formatter);
}

void Metrics::Calculate(Graphics& graphics)
{
    dpiX = graphics.GetDpiX();
    dpiY = graphics.GetDpiY();
    defaultButtonSize.Width = MMToHorizontalPixels(defaultButtonSizeElement.Get().Width);
    defaultButtonSize.Height = MMToVerticalPixels(defaultButtonSizeElement.Get().Height);
    defaultLabelSize.Width = MMToHorizontalPixels(defaultLabelSizeElement.Get().Width);
    defaultLabelSize.Height = MMToVerticalPixels(defaultLabelSizeElement.Get().Height);
    defaultTextBoxSize.Width = MMToHorizontalPixels(defaultTextBoxSizeElement.Get().Width);
    defaultTextBoxSize.Height = MMToVerticalPixels(defaultTextBoxSizeElement.Get().Height);
    defaultListBoxSize.Width = MMToHorizontalPixels(defaultListBoxSizeElement.Get().Width);
    defaultListBoxSize.Height = MMToVerticalPixels(defaultListBoxSizeElement.Get().Height);
    defaultCheckBoxSize.Width = MMToHorizontalPixels(defaultCheckBoxSizeElement.Get().Width);
    defaultCheckBoxSize.Height = MMToVerticalPixels(defaultCheckBoxSizeElement.Get().Height);
    defaultControlSpacing.Width = MMToHorizontalPixels(defaultControlSpacingElement.Get().Width);
    defaultControlSpacing.Height = MMToVerticalPixels(defaultControlSpacingElement.Get().Height);
}

void Metrics::Read(sngxml::dom::Element* parentElement)
{
    defaultButtonSizeElement.Read(parentElement);
    defaultLabelSizeElement.Read(parentElement);
    defaultTextBoxSizeElement.Read(parentElement);
    defaultListBoxSizeElement.Read(parentElement);
    defaultCheckBoxSizeElement.Read(parentElement);
    defaultControlSpacingElement.Read(parentElement);
}

void Metrics::Write(sngxml::dom::Element* parentElement)
{
    defaultButtonSizeElement.Write(parentElement);
    defaultLabelSizeElement.Write(parentElement);
    defaultTextBoxSizeElement.Write(parentElement);
    defaultListBoxSizeElement.Write(parentElement);
    defaultCheckBoxSizeElement.Write(parentElement);
    defaultControlSpacingElement.Write(parentElement);
}

Metrics ScreenMetrics::metrics;

void ScreenMetrics::Load()
{
    metrics.LoadFromFile(GetDefaultMetricsFilePath());
}

void ScreenMetrics::Save(bool setElementValues)
{
    metrics.SaveToFile(GetDefaultMetricsFilePath(), setElementValues);
}

void ScreenMetrics::Calculate(Graphics& graphics)
{
    Load();
    metrics.Calculate(graphics);
}

void ScreenMetrics::Set(Metrics& metrics_)
{
    metrics = metrics_;
}

void LoadMetrics()
{
    ScreenMetrics::Load();
}

} } // cmajor::wing
