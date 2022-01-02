// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_DEBUG_VIEW_INCLUDED
#define CMSX_DB_DEBUG_VIEW_INCLUDED
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::db {

class DebugView
{
public: 
    DebugView();
    virtual ~DebugView();
    virtual void UpdateView();
    virtual void SetMachine(cmsx::machine::Machine* machine);
    virtual void SetProcess(cmsx::kernel::Process* process);
    virtual void WriteLine(const std::string& logMessage);
    virtual void Clear();
    virtual void ToStart();
    virtual void ToEnd();
    virtual void NextPage();
    virtual void PrevPage();
    virtual void NextLine();
    virtual void PrevLine();
    virtual void NextQuarter();
    virtual void PrevQuarter();
    void SetVisible(bool visible_) { visible = visible_; }
    bool IsVisible() const { return visible; }
private:
    bool visible;
};

} // namespace cmsx::db

#endif // CMSX_DB_DEBUG_VIEW_INCLUDED
