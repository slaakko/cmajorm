// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_LOG_VIEW_INCLUDED
#define CMAJOR_WING_LOG_VIEW_INCLUDED
#include <wing/TextView.hpp>

namespace cmajor { namespace wing {

class WING_API LogView : public TextView
{
public:
    LogView(TextViewCreateParams& createParams);
    virtual void WriteLine(const std::string& text);
};

} } // cmajor::wing

#endif // CMAJOR_WING_LOG_VIEW_INCLUDED
