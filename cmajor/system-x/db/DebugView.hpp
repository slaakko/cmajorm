// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_DEBUG_VIEW_INCLUDED
#define CMSX_DB_DEBUG_VIEW_INCLUDED
#include <system-x/os/Process.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::db {

class DebugView
{
public: 
    DebugView();
    virtual ~DebugView();
    virtual void UpdateView() = 0;
    virtual void SetMachine(cmsx::machine::Machine* machine) = 0;
    virtual void SetProcess(cmsx::os::Process* process) = 0;
    virtual void ToStart() = 0;
    virtual void ToEnd() = 0;
    virtual void NextPage() = 0;
    virtual void PrevPage() = 0;
    virtual void NextLine() = 0;
    virtual void PrevLine() = 0;
    virtual void NextQuarter() = 0;
    virtual void PrevQuarter() = 0;
    void SetVisible(bool visible_) { visible = visible_; }
    bool IsVisible() const { return visible; }
private:
    bool visible;
};

} // namespace cmsx::db

#endif // CMSX_DB_DEBUG_VIEW_INCLUDED
