// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <db/DebugView.hpp>

namespace cmsx::db {

DebugView::DebugView() : visible(false)
{
}

DebugView::~DebugView()
{
}

void DebugView::UpdateView()
{
}

void DebugView::SetMachine(cmsx::machine::Machine* machine)
{
}

void DebugView::SetProcess(cmsx::kernel::Process* process)
{
}

void DebugView::WriteLine(const std::string& logMessage)
{
}

void DebugView::Clear()
{
}

void DebugView::ToStart()
{
}

void DebugView::ToEnd()
{
}

void DebugView::NextPage()
{
}

void DebugView::PrevPage()
{
}

void DebugView::NextLine()
{
}

void DebugView::PrevLine()
{
}

void DebugView::NextQuarter()
{
}

void DebugView::PrevQuarter()
{
}

} // namespace cmsx::db
