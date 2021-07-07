#include <cmview/CodeCompletionView.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <boost/lexical_cast.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>

#undef min
#undef max

namespace cmajor { namespace view {

using namespace soulng::unicode;

CCEntry::CCEntry() : prefixLength(0), category(), completion(), help(), replacement(), symbolIndex(-1)
{
}

std::vector<CCEntry> ParseCCList(const std::string& ccList)
{
    std::vector<CCEntry> ccEntries;
    std::unique_ptr<sngxml::dom::Document> doc = sngxml::dom::ParseDocument(ToUtf32(ccList), "ccList");
    std::unique_ptr<sngxml::xpath::XPathObject> object = sngxml::xpath::Evaluate(U"/ccList/symbol", doc.get());
    if (object)
    {
        if (object->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(object.get());
            int n = nodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                CCEntry entry;
                sngxml::dom::Node* node = (*nodeSet)[i];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                    std::u32string prefixLengthAttr = element->GetAttribute(U"prefixLength");
                    if (prefixLengthAttr.empty())
                    {
                        throw std::runtime_error("prefixLength attribute is empty");
                    }
                    entry.prefixLength = boost::lexical_cast<int>(ToUtf8(prefixLengthAttr));
                    std::u32string categoryAttr = element->GetAttribute(U"category");
                    if (categoryAttr.empty())
                    {
                        throw std::runtime_error("category attribute is empty");
                    }
                    entry.category = ToUtf8(categoryAttr);
                    std::u32string helpAttr = element->GetAttribute(U"help");
                    entry.help = ToUtf8(helpAttr);
                    std::u32string completionAttr = element->GetAttribute(U"completion");
                    if (completionAttr.empty())
                    {
                        throw std::runtime_error("completion attribute is empty");
                    }
                    entry.completion = ToUtf8(completionAttr);
                    std::u32string replacementAttr = element->GetAttribute(U"replacement");
                    if (replacementAttr.empty())
                    {
                        throw std::runtime_error("replacement attribute is empty");
                    }
                    entry.replacement = ToUtf8(replacementAttr);
                    std::u32string symbolIndexAttr = element->GetAttribute(U"symbolIndex");
                    if (symbolIndexAttr.empty())
                    {
                        throw std::runtime_error("symbol index attribute is empty");
                    }
                    entry.symbolIndex = boost::lexical_cast<int>(ToUtf8(symbolIndexAttr));
                    ccEntries.push_back(entry);
                }
            }
        }
    }
    return ccEntries;
}

Color DefaultCCListNormalBackgroundColor()
{
    return Color(245, 245, 245);
}

Color DefaultCCListSelectedBackgroundColor()
{
    return Color(0, 122, 204);
}

Color DefaultCCListNormalTextColor()
{
    return Color::Black;
}

Color DefaultCCListSelectedTextColor()
{
    return Color::White;
}

std::string DefaultCCListFontFamilyName()
{
    return "Segoe UI";
}

float DefaultCCListFontSize()
{
    return 9.0f;
}

CCListCreateParams::CCListCreateParams() : controlCreateParams(), fontFamilyName(), fontSize()
{
    controlCreateParams.WindowClassName("cmajor.view.CCList");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(DefaultCCListNormalBackgroundColor());
    fontFamilyName = DefaultCCListFontFamilyName();
    fontSize = DefaultCCListFontSize();
    selectedBackgroundColor = DefaultCCListSelectedBackgroundColor();
    normalTextColor = DefaultCCListNormalTextColor();
    selectedTextColor = DefaultCCListSelectedTextColor();
}

CCListCreateParams& CCListCreateParams::Defaults()
{
    return *this;
}

CCListCreateParams& CCListCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

CCListCreateParams& CCListCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return* this;
}

CCListCreateParams& CCListCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

CCListCreateParams& CCListCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

CCListCreateParams& CCListCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

CCListCreateParams& CCListCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

CCListCreateParams& CCListCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

CCListCreateParams& CCListCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

CCListCreateParams& CCListCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

CCListCreateParams& CCListCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

CCListCreateParams& CCListCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

CCListCreateParams& CCListCreateParams::FontSize(float fontSize_)
{
    fontSize = fontSize_;
    return *this;
}

CCListCreateParams& CCListCreateParams::SelectedBackgroundColor(const Color& color)
{
    selectedBackgroundColor = color;
    return *this;
}

CCListCreateParams& CCListCreateParams::NormalTextColor(const Color& color)
{
    normalTextColor = color;
    return *this;
}

CCListCreateParams& CCListCreateParams::SelectedTextColor(const Color& color)
{
    selectedTextColor = color;
    return *this;
}

CCList::CCList(CCListCreateParams& createParams) :
    Control(createParams.controlCreateParams),
    flags(CCListFlags::none),
    selectedBackgroundColor(createParams.selectedBackgroundColor),
    normalTextColor(createParams.normalTextColor),
    selectedTextColor(createParams.selectedTextColor),
    topEntryIndex(0),
    selectedEntryIndex(-1),
    stringFormat(),
    normalBackgroundBrush(BackgroundColor()),
    selectedBackgroundBrush(selectedBackgroundColor),
    normalTextBrush(normalTextColor),
    selectedTextBrush(selectedTextColor),
    lineHeight(0),
    charWidth(0)
{
    SetChanged();
    std::u16string fontFamName = ToUtf16(createParams.fontFamilyName);
    FontFamily family((const WCHAR*)fontFamName.c_str());
    SetFont(Font(family, createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
}

void CCList::SetContent(const std::vector<CCEntry>& entries_)
{
    entries = entries_;
    Size contentSize = GetSize();
    contentSize.Height = static_cast<int>(entries.size() * static_cast<int>(lineHeight + 0.5));
    topEntryIndex = 0;
    selectedEntryIndex = 0;
    SetContentLocation();
    SetContentSize(contentSize);
    Invalidate();
}

Size CCList::DefaultSize() const
{
    return Size(static_cast<int>(charWidth * 80), static_cast<int>(defaultNumberOfCCLines * lineHeight));
}

bool CCList::Changed() const
{
    return (flags & CCListFlags::changed) != CCListFlags::none;
}

void CCList::SetChanged()
{
    flags = flags | CCListFlags::changed;
}

void CCList::ResetChanged()
{
    flags = flags & ~CCListFlags::changed;
}

bool CCList::Measured() const
{
    return (flags & CCListFlags::measured) != CCListFlags::none;
}

void CCList::SetMeasured()
{
    flags = flags | CCListFlags::measured;
}

void CCList::ResetMeasured()
{
    flags = flags & ~CCListFlags::measured;
}

void CCList::Next()
{
    if (selectedEntryIndex == -1)
    {
        selectedEntryIndex = 0;
        topEntryIndex = 0;
    }
    else if (selectedEntryIndex < entries.size() - 1)
    {
        ++selectedEntryIndex;
        int n = GetNumberOfVisibleLines();
        if (topEntryIndex <= selectedEntryIndex - n)
        {
            topEntryIndex = std::min(static_cast<int>(entries.size() - 1), selectedEntryIndex - n + 1);
        }
        SetContentLocation();
        Invalidate();
    }
}

void CCList::Prev()
{
    if (selectedEntryIndex == -1)
    {
        selectedEntryIndex = 0;
        topEntryIndex = 0;
    }
    else if (selectedEntryIndex > 0)
    {
        --selectedEntryIndex;
        if (topEntryIndex > selectedEntryIndex)
        {
            topEntryIndex = selectedEntryIndex;
        }
    }
    SetContentLocation();
    Invalidate();
}

void CCList::NextPage()
{
    if (selectedEntryIndex == -1)
    {
        selectedEntryIndex = 0;
        topEntryIndex = 0;
    }
    else if (selectedEntryIndex < entries.size() - 1)
    {
        int n = GetNumberOfVisibleLines();
        selectedEntryIndex = std::min(static_cast<int>(entries.size() - 1), selectedEntryIndex + n);
        topEntryIndex = std::max(static_cast<int>(0), selectedEntryIndex - n + 1);
    }
    SetContentLocation();
    Invalidate();
}

void CCList::PrevPage()
{
    if (selectedEntryIndex == -1)
    {
        selectedEntryIndex = 0;
        topEntryIndex = 0;
    }
    else if (selectedEntryIndex > 0)
    {
        int n = GetNumberOfVisibleLines();
        selectedEntryIndex = std::max(static_cast<int>(0), selectedEntryIndex - n);
        if (topEntryIndex > selectedEntryIndex)
        {
            topEntryIndex = selectedEntryIndex;
        }
    }
    SetContentLocation();
    Invalidate();
}

const CCEntry& CCList::GetSelectedEntry() const
{
    if (selectedEntryIndex >= 0 && selectedEntryIndex < entries.size())
    {
        return entries[selectedEntryIndex];
    }
    else
    {
        throw std::runtime_error("invalid code completion list entry index");
    }
}

void CCList::OnPaint(PaintEventArgs& args)
{
    if (Changed())
    {
        ResetChanged();
        Measure(args.graphics);
    }
    args.graphics.Clear(BackgroundColor());
    Size size = GetSize();
    int n = topEntryIndex + GetNumberOfVisibleLines();
    for (int i = topEntryIndex; i < n; ++i)
    {
        std::string line;
        if (i >= 0 && i < entries.size())
        {
            const CCEntry& entry = entries[i];
            line.append(entry.category).append(1, ' ').append(entry.completion);
        }
        PointF origin(0, static_cast<int>(lineHeight + 0.5) * i);
        SizeF sz(size.Width, lineHeight + 0.5f);
        RectF rect(origin, sz);
        Brush* backgroundBrush = &normalBackgroundBrush;
        Brush* textBrush = &normalTextBrush;
        if (i == selectedEntryIndex)
        {
            backgroundBrush = &selectedBackgroundBrush;
            textBrush = &selectedTextBrush;
        }
        args.graphics.FillRectangle(backgroundBrush, rect);
        DrawString(args.graphics, line, GetFont(), origin, *textBrush);
    }
}

void CCList::Measure(Graphics& graphics)
{
    std::string testString = "This is a test string";
    PointF origin;
    RectF nr = MeasureString(graphics, testString, GetFont(), origin, stringFormat);
    lineHeight = nr.Height;
    charWidth = nr.Width / testString.length();
    SetScrollUnits(ScrollUnits(static_cast<int>(lineHeight + 0.5), static_cast<int>(2 * (charWidth + 0.5))));
    SetMeasured();
}

int CCList::GetNumberOfVisibleLines() const
{
    Size size = GetSize();
    return static_cast<int>(size.Height / static_cast<int>(lineHeight + 0.5));
}

void CCList::SetContentLocation()
{
    Control::SetContentLocation(Point(0, topEntryIndex * static_cast<int>(lineHeight + 0.5)));
}

Color DefaultCodeCompletionListViewBackgroundColor()
{
    return Color(245, 245, 245);
}

Color DefaultCodeCompletionListViewBorderColor()
{
    return Color(204, 206, 219);
}

Padding DefaultCodeCompletionListViewPadding()
{
    return Padding(4, 4, 4, 4);
}

BorderStyle DefaultCodeCompletionListViewBorderStyle()
{
    return BorderStyle::single;
}

CodeCompletionListViewCreateParams::CodeCompletionListViewCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("cmajor.view.CodeCompletionListView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(DefaultCodeCompletionListViewBackgroundColor());
    borderColor = DefaultCodeCompletionListViewBorderColor();
    padding = DefaultCodeCompletionListViewPadding();
    borderStyle = DefaultCodeCompletionListViewBorderStyle();
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::Defaults()
{
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::BorderColor(const Color& color)
{
    borderColor = color;
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::SetPadding(const Padding& padding_)
{
    padding = padding_;
    return *this;
}

CodeCompletionListViewCreateParams& CodeCompletionListViewCreateParams::SetBorderStyle(BorderStyle borderStyle_)
{
    borderStyle = borderStyle_;
    return *this;
}

CodeCompletionListView::CodeCompletionListView(CCListCreateParams& ccListCreateParams, CodeCompletionListViewCreateParams& createParams) : 
    Control(createParams.controlCreateParams),
    borderColor(createParams.borderColor),
    padding(createParams.padding),
    borderStyle(createParams.borderStyle),
    container(this),
    ccList(nullptr),
    child(nullptr)
{
    std::unique_ptr<CCList> ccListPtr(new CCList(ccListCreateParams));
    ccList = ccListPtr.get();
    ccList->SetDoubleBuffered();
    std::unique_ptr<PaddedControl> paddedControlPtr(new PaddedControl(PaddedControlCreateParams(ccListPtr.release()).SetPadding(padding).BackgroundColor(ccList->BackgroundColor())));
    std::unique_ptr<ScrollableControl> scrollableControlPtr(new ScrollableControl(ScrollableControlCreateParams(paddedControlPtr.release()).BackgroundColor(ccList->BackgroundColor())));
    std::unique_ptr<BorderedControl> borderedControlPtr(new BorderedControl(BorderedControlCreateParams(scrollableControlPtr.release()).SetBorderStyle(borderStyle).
        SetDock(Dock::fill).FocusedSingleBorderColor(createParams.borderColor).NormalSingleBorderColor(createParams.borderColor).BackgroundColor(ccList->BackgroundColor())));
    child = borderedControlPtr.get();
    container.AddChild(borderedControlPtr.release());
    SetSize(Size(100, 100));
}

void CodeCompletionListView::SetDefaultSize()
{
    if (ccList->Measured())
    {
        ccList->ResetMeasured();
        Size sz = ccList->DefaultSize();
        Size ps(PaddedSize(Size(sz.Width, sz.Height), padding));
        Size bs(BorderedSize(Size(ps.Width, ps.Height), borderStyle));
        SetSize(bs);
    }
}

void CodeCompletionListView::SetContent(const std::vector<CCEntry>& entries)
{
    ccList->SetContent(entries);
}

void CodeCompletionListView::Next()
{
    ccList->Next();
}

void CodeCompletionListView::Prev()
{
    ccList->Prev();
}

void CodeCompletionListView::NextPage()
{
    ccList->NextPage();
}

void CodeCompletionListView::PrevPage()
{
    ccList->PrevPage();
}

const CCEntry& CodeCompletionListView::GetSelectedEntry() const
{
    return ccList->GetSelectedEntry();
}

void CodeCompletionListView::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void CodeCompletionListView::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void CodeCompletionListView::SetChildPos()
{
    child->SetLocation(Point());
    child->SetSize(GetSize());
}

void CodeCompletionListView::OnPaint(PaintEventArgs& args)
{
    child->Invalidate();
    ccList->Invalidate();
}

} } // namespace cmajor::view
