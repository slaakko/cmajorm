// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMPMV_MAIN_WINDOW_INCLUDED
#define CMPMV_MAIN_WINDOW_INCLUDED
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/LogView.hpp>
#include <cmajor/cmview/PortMapView.hpp>

namespace cmajor { namespace pmv {

using namespace cmajor::view;
using namespace cmajor::wing;

class MainWindow : public Window
{
public:
    MainWindow();
    ~MainWindow();
    void HandleServiceMessage();
protected:
    bool ProcessMessage(Message& msg) override;
    void OnTimer(TimerEventArgs& args) override;
private:
    void ClearOutput();
    void WriteOutput(const std::string& text);
    void Exit();
    void Start();
    void Stop();
    void Update();
    PortMapView* portMapView;
    LogView* log;
    MenuItem* startMenuItem;
    MenuItem* stopMenuItem;
    MenuItem* updateMenuItem;
};

} } // namespace cmajor::pmv

#endif // CMPMV_MAIN_WINDOW_INCLUDED
