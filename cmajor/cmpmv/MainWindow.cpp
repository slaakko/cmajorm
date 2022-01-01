// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpmv/MainWindow.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>
#include <cmajor/cmview/PortMapView.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/RequestDispatcher.hpp>
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <cmajor/wing/SplitContainer.hpp>

namespace cmajor { namespace pmv {

using namespace cmajor::view;
using namespace cmajor::service;

MainWindow::MainWindow() : Window(WindowCreateParams().WindowClassName("cmajor.pmv.MainWindow").Text("Cmajor Port Map Monitor")), 
    log(nullptr), startMenuItem(nullptr), stopMenuItem(nullptr), updateMenuItem(nullptr), portMapView(nullptr)
{
    std::unique_ptr<MenuBar> menuBar(new MenuBar());
    std::unique_ptr<MenuItem> fileMenuItem(new MenuItem("&File"));
    std::unique_ptr<MenuItem> exitMenuItem(new MenuItem("E&xit"));
    exitMenuItem->SetShortcut(Keys::altModifier | Keys::f4);
    exitMenuItem->Click().AddHandler(this, &MainWindow::Exit);
    fileMenuItem->AddMenuItem(exitMenuItem.release());
    menuBar->AddMenuItem(fileMenuItem.release());
    std::unique_ptr<MenuItem> serverMenuItem(new MenuItem("Server"));
    std::unique_ptr<MenuItem> startMenuItemPtr(new MenuItem("&Start"));
    startMenuItem = startMenuItemPtr.get();
    startMenuItem->Click().AddHandler(this, &MainWindow::Start);
    startMenuItem->SetShortcut(Keys::f2);
    serverMenuItem->AddMenuItem(startMenuItemPtr.release());
    std::unique_ptr<MenuItem> stopMenuItemPtr(new MenuItem("Sto&p"));
    stopMenuItem = stopMenuItemPtr.get();
    stopMenuItem->Click().AddHandler(this, &MainWindow::Stop);
    stopMenuItem->SetShortcut(Keys::f3);
    serverMenuItem->AddMenuItem(stopMenuItemPtr.release());
    menuBar->AddMenuItem(serverMenuItem.release());
    std::unique_ptr<MenuItem> windowMenuItem(new MenuItem("Window"));
    std::unique_ptr<MenuItem> updateMenuItemPtr(new MenuItem("&Update"));
    updateMenuItem = updateMenuItemPtr.get();
    updateMenuItem->Click().AddHandler(this, &MainWindow::Update);
    updateMenuItem->SetShortcut(Keys::f5);
    windowMenuItem->AddMenuItem(updateMenuItemPtr.release());
    menuBar->AddMenuItem(windowMenuItem.release());
    AddChild(menuBar.release());
    std::unique_ptr<SplitContainer> splitContainer(new SplitContainer(SplitContainerCreateParams(SplitterOrientation::vertical).SplitterDistance(0).SetDock(Dock::fill)));
    std::unique_ptr<PortMapView> portMapViewPtr(new PortMapView());
    portMapView = portMapViewPtr.get();
    splitContainer->Pane1Container()->AddChild(portMapViewPtr.release());
    std::unique_ptr<LogView> logView(new LogView(TextViewCreateParams().Defaults()));
    log = logView.get();
    log->SetFlag(ControlFlags::scrollSubject);
    std::unique_ptr<PaddedControl> paddedLogView(new PaddedControl(PaddedControlCreateParams(logView.release()).Defaults()));
    std::unique_ptr<ScrollableControl> scrollableLog(new ScrollableControl(ScrollableControlCreateParams(paddedLogView.release()).SetDock(Dock::fill)));
    splitContainer->Pane2Container()->AddChild(scrollableLog.release());
    AddChild(splitContainer.release());
    SetServiceMessageHandlerView(this);
    StartRequestDispatcher();
    Update();
    SetTimer(100, 10000);
}

MainWindow::~MainWindow()
{
    SetServiceMessageHandlerView(nullptr);
    StopRequestDispatcher();
}

bool MainWindow::ProcessMessage(Message& msg)
{
    switch (msg.message)
    {
        case SM_SERVICE_MESSAGE_AVAILABLE:
        {
            HandleServiceMessage();
            msg.result = 0;
            return true;
        }
        default:
        {
            return Window::ProcessMessage(msg);
        }
    }
}

void MainWindow::OnTimer(TimerEventArgs& args)
{
    Update();
}

void MainWindow::HandleServiceMessage()
{
    std::unique_ptr<ServiceMessage> serviceMessage = GetServiceMessage();
    if (serviceMessage)
    {
        switch (serviceMessage->Kind())
        {
            case ServiceMessageKind::clearOutput:
            {
                ClearOutput();
                break;
            }
            case ServiceMessageKind::outputMessage:
            {
                OutputServiceMessage* message = static_cast<OutputServiceMessage*>(serviceMessage.get());
                WriteOutput(message->Text());
                break;
            }
        }
    }
}

void MainWindow::ClearOutput()
{
    log->Clear();
}

void MainWindow::WriteOutput(const std::string& text)
{
    log->WriteLine(text);
}

void MainWindow::Exit()
{
    Close();
}

void MainWindow::Start()
{
    if (StartPortMapServer())
    {
        startMenuItem->Disable();
        stopMenuItem->Enable();
        PutOutputServiceMessage("port map server (cmpms) started");
    }
}

void MainWindow::Stop()
{
    if (StopPortMapServer())
    {
        startMenuItem->Enable();
        stopMenuItem->Disable();
        portMapView->Clear();
        PutOutputServiceMessage("port map server (cmpms) stopped");
    }
}

void MainWindow::Update()
{
    if (IsPortMapServerRunning())
    {
        startMenuItem->Disable();
        stopMenuItem->Enable();
        std::vector<PortLease> portLeases = GetPortLeases();
        portMapView->Update(portLeases);
    }
    else
    {
        startMenuItem->Enable();
        stopMenuItem->Disable();
        PutOutputServiceMessage("port map server (cmpms) not running");
    }
}

} } // namespace cmajor::pmv
