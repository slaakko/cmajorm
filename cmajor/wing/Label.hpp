// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_LABEL_INCLUDED
#define CMAJOR_WING_LABEL_INCLUDED
#include <cmajor/wing/Control.hpp>

namespace cmajor { namespace wing {

struct WING_API LabelCreateParams
{
    LabelCreateParams();
    LabelCreateParams& Defaults();
    LabelCreateParams& WindowClassName(const std::string& windowClassName_);
    LabelCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    LabelCreateParams& WindowStyle(int windowStyle_);
    LabelCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    LabelCreateParams& BackgroundColor(const Color& backgroundColor_);
    LabelCreateParams& Text(const std::string& text_);
    LabelCreateParams& Location(Point location_);
    LabelCreateParams& SetSize(Size size_);
    LabelCreateParams& SetAnchors(Anchors anchors_);
    LabelCreateParams& SetDock(Dock dock_);
    LabelCreateParams& AutoSize(bool autoSize_);
    ControlCreateParams controlCreateParams;
    bool autoSize;
};

enum class LabelFlags : int 
{
    none = 0, autoSize = 1 << 0, autoSized = 1 << 1
};

inline WING_API LabelFlags operator|(LabelFlags left, LabelFlags right)
{
    return LabelFlags(int(left) | int(right));
}

inline WING_API LabelFlags operator&(LabelFlags left, LabelFlags right)
{
    return LabelFlags(int(left) & int(right));
}

inline WING_API LabelFlags operator~(LabelFlags flags)
{
    return LabelFlags(~int(flags));
}

class WING_API Label : public Control
{
public:
    Label(LabelCreateParams& createParams);
protected:
    void OnCreated() override;
    void OnTextChanged() override;
private:
    void DoAutoSize();
    LabelFlags flags;
};

} } // cmajor::wing

#endif // CMAJOR_WING_LABEL_INCLUDED
