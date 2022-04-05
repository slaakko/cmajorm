// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Terminal.hpp>
#include <system-x/kernel/OsApi.hpp>
#include <system-x/kernel/EventManager.hpp>
#include <system-x/kernel/Debug.hpp>
#include <soulng/util/Unicode.hpp>
#include <memory>
#include <list>
#include <thread>
#include <stack>

namespace cmsx::kernel {

using namespace soulng::unicode;

enum class TerminalMode : int32_t
{
    cooked = 0, raw = 1
};

bool IsPrintChar(char32_t ch)
{
    return ch >= 32 && ch < static_cast<char32_t>(specialKeyStart);
}

class Terminal
{
public:
    static void Init();
    static void Done();
    static bool Initialized() { return initialized; }
    static Terminal& Instance() { return *instance; }
    int32_t CursorPosX() const { return cursorPosX; }
    int32_t CursorPosY() const { return cursorPosY; }
    int32_t ScreenSizeX() const { return screenSizeX; }
    int32_t ScreenSizeY() const { return screenSizeY; }
    void SetCursorPos(int x, int y);
    void GetTerminalInput();
    void SetCooked() { mode = TerminalMode::cooked; }
    void SetRaw() { mode = TerminalMode::raw; }
    void SetEcho(bool echo_) { echo = echo_; }
    void PushLines();
    void PopLines();
    std::vector<uint8_t> Read(int64_t count, cmsx::machine::Process* process);
    int64_t Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process);
    void SetMachine(cmsx::machine::Machine* machine_) { machine = machine_; }
    void SendKey(char32_t key);
    void Start();
    void Stop();
    void Run();
    ~Terminal();
private:
    Terminal();
    void Write(const std::u32string& utf32Chars);
    void GetScreenBufferInfo();
    void HandleInputChar(char32_t ch);
    void PrintChar(char32_t ch);
    void PrintNewLine();
    void HandleChar(char32_t ch);
    void HandleEscape();
    void HandleNewLine();
    void HandleBackspace();
    void HandleTab();
    void HandleEof();
    void HandleHome();
    void HandleEnd();
    void HandleLeft();
    void HandleRight();
    void HandleUp();
    void HandleDown();
    void HandleControlLeft();
    void HandleControlRight();
    void HandleDel();
    void ClearUpdateLine();
    void UpdateLine();
    void UpdateCursorPos();
    cmsx::machine::Machine* machine;
    static std::unique_ptr<Terminal> instance;
    static bool initialized;
    void* consoleInputHandle;
    void* consoleOutputHandle;
    void* consoleErrorHandle;
    void* waitHandle;
    int32_t cursorPosX;
    int32_t cursorPosY;
    int32_t outputEndCursorPosX;
    int32_t outputEndCursorPosY;
    int32_t screenSizeX;
    int32_t screenSizeY;
    uint16_t attrs;
    uint16_t defaultAttrs;
    TerminalFile terminalFile;
    TerminalMode mode;
    bool echo;
    std::list<char32_t> terminalInputQueue;
    std::list<uint8_t> terminalInputBuffer;
    cmsx::machine::Event terminalInputEvent;
    std::thread terminalThread;
    bool sleepingOnTerminalInputEvent;
    bool terminalInputReady;
    bool exiting;
    std::condition_variable_any terminalEventVar;
    bool started;
    bool stopped;
    Utf8ToUtf32Engine utfEngine;
    std::vector<std::u32string> lines;
    std::stack<std::vector<std::u32string>> linesStack;
    std::u32string line;
    int pos;
    int lineIndex;
    bool eof;
};

bool Terminal::initialized = false;

 void RunTerminal()
{
    Terminal::Instance().Run();
}

void Terminal::Start()
{
    terminalThread = std::thread(RunTerminal);
    started = true;
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("terminal started");
    }
}

void Terminal::Stop()
{
    if (!started) return;
    stopped = true;
    if (waitHandle)
    {
        OsUnregisterConsoleCallBack(waitHandle);
    }
    exiting = true;
    terminalEventVar.notify_one();
    if (sleepingOnTerminalInputEvent)
    {
        Wakeup(terminalInputEvent);
    }
    terminalThread.join();
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("terminal stopped");
    }
}

void Terminal::Run()
{
    while (!exiting && !machine->Exiting())
    {
        std::unique_lock<std::recursive_mutex> lock(machine->Lock());
        terminalEventVar.wait(lock, [this] { return terminalInputReady || exiting || machine->Exiting(); });
        terminalInputReady = false;
        if (exiting || machine->Exiting()) return;
        char32_t ch = terminalInputQueue.front();
        terminalInputQueue.pop_front();
        HandleInputChar(ch);
    }
}

void Terminal::GetTerminalInput()
{
    char32_t ch = OsReadConsoleInput(consoleInputHandle);
    if (ch)
    {
        if ((GetDebugMode() & debugTerminalMode) != 0)
        {
            DebugWrite("read.key(" + OsKeyName(ch) + ")");
        }
        terminalInputQueue.push_back(ch);
        terminalInputReady = true;
        terminalEventVar.notify_one();
    }
}

void Terminal::SendKey(char32_t key)
{
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
    }
    terminalInputQueue.push_back(key);
    terminalInputReady = true;
    terminalEventVar.notify_one();
}

void Terminal::PushLines()
{
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("> push.lines: " + std::to_string(lines.size()));
    }
    linesStack.push(std::move(lines));
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("< push.lines");
    }
}

void Terminal::PopLines()
{
    if (!linesStack.empty())
    {
        lines = std::move(linesStack.top());
        lineIndex = lines.size();
        if ((GetDebugMode() & debugTerminalMode) != 0)
        {
            DebugWrite("> pop.lines: " + std::to_string(lines.size()) + ", line index=" + std::to_string(lineIndex));
        }
        linesStack.pop();
    }
    else
    {
        if ((GetDebugMode() & debugTerminalMode) != 0)
        {
            DebugWrite("> pop.lines: lines stack is empty");
        }
    }
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("< pop.lines");
    }
}

void TerminalInputWaiting()
{
    Terminal::Instance().GetTerminalInput();
}

std::unique_ptr<Terminal> Terminal::instance;

void Terminal::Init()
{
    instance.reset(new Terminal());
    initialized = true;
}

void Terminal::Done()
{
    instance.reset();
}

Terminal::Terminal() :
    consoleInputHandle(OsGetStdHandle(0)),
    consoleOutputHandle(OsGetStdHandle(1)),
    consoleErrorHandle(OsGetStdHandle(2)),
    waitHandle(nullptr),
    cursorPosX(0),
    cursorPosY(0),
    screenSizeX(0),
    screenSizeY(0),
    outputEndCursorPosX(0),
    outputEndCursorPosY(0),
    attrs(0),
    defaultAttrs(0),
    mode(TerminalMode::cooked),
    echo(true),
    terminalInputEvent(cmsx::machine::EventKind::terminalInputEvent, 0),
    sleepingOnTerminalInputEvent(false),
    terminalInputReady(false),
    exiting(false),
    started(false),
    stopped(false),
    pos(0),
    lineIndex(0),
    eof(false)
{
    GetScreenBufferInfo();
    defaultAttrs = attrs;
    OsRegisterConsoleCallBack(consoleInputHandle, &TerminalInputWaiting, waitHandle);
    SetTerminalFile(&terminalFile);
}

Terminal::~Terminal()
{
    if (!stopped)
    {
        Stop();
    }
}

void Terminal::GetScreenBufferInfo()
{
    OsGetConsoleScreenBufferInfo(consoleOutputHandle, &cursorPosX, &cursorPosY, &screenSizeX, &screenSizeY, &attrs);
}

void Terminal::SetCursorPos(int x, int y)
{
    OsSetConsoleCursorPosition(consoleOutputHandle, x, y);
    GetScreenBufferInfo();
}

void Terminal::PrintChar(char32_t ch)
{
    std::u32string chars(1, ch);
    OsWriteConsole(consoleOutputHandle, chars.c_str());
    GetScreenBufferInfo();
}

void Terminal::PrintNewLine()
{
    std::u32string chars(1, '\n');
    OsWriteConsole(consoleOutputHandle, chars.c_str());
    GetScreenBufferInfo();
}

void Terminal::HandleInputChar(char32_t ch)
{
    if (mode == TerminalMode::raw)
    {
        if (echo)
        {
            if (IsPrintChar(ch))
            {
                PrintChar(ch);
            }
            else if (ch == '\n')
            {
                PrintNewLine();
            }
        }
        if (ch == keyControlD)
        {
            HandleEof();
        }
        else
        {
            std::string chars = ToUtf8(std::u32string(1, ch));
            for (char c : chars)
            {
                terminalInputBuffer.push_back(static_cast<uint8_t>(c));
            }
        }
        Wakeup(terminalInputEvent);
    }
    else if (mode == TerminalMode::cooked)
    {
        if (ch < 32)
        {
            switch (ch)
            {
                case '\n':
                {
                    HandleNewLine();
                    break;
                }
                case static_cast<char32_t>(keyBackspace):
                {
                    HandleBackspace();
                    break;
                }
                case static_cast<char32_t>(keyTab):
                {
                    HandleTab();
                    break;
                }
                case static_cast<char32_t>(keyControlD):
                {
                    HandleEof();
                    break;
                }
                case static_cast<char32_t>(keyEscape):
                {
                    HandleEscape();
                    break;
                }
            }
        }
        else if (ch >= specialKeyStart && ch <= specialKeyEnd)
        {
            switch (ch)
            {
                case static_cast<char32_t>(keyHome):
                {
                    HandleHome();
                    break;
                }
                case static_cast<char32_t>(keyEnd):
                {
                    HandleEnd();
                    break;
                }
                case static_cast<char32_t>(keyLeft):
                {
                    HandleLeft();
                    break;
                }
                case static_cast<char32_t>(keyRight):
                {
                    HandleRight();
                    break;
                }
                case static_cast<char32_t>(keyUp):
                {
                    HandleUp();
                    break;
                }
                case static_cast<char32_t>(keyDown):
                {
                    HandleDown();
                    break;
                }
                case static_cast<char32_t>(keyControlLeft):
                {
                    HandleControlLeft();
                    break;
                }
                case static_cast<char32_t>(keyControlRight):
                {
                    HandleControlRight();
                    break;
                }
                case static_cast<char32_t>(keyDel):
                {
                    HandleDel();
                    break;
                }
            }
        }
        else
        {
            HandleChar(ch);
        }
    }
}
void Terminal::HandleChar(char32_t ch)
{
    if (pos < line.length())
    {
        line = line.substr(0, pos) + std::u32string(1, ch) + line.substr(pos);
    }
    else
    {
        line.append(1, ch);
    }
    ++pos;
    UpdateLine();
    UpdateCursorPos();
}

void Terminal::HandleEscape()
{
    ClearUpdateLine();
    line.clear();
    pos = 0;
    UpdateLine();
    UpdateCursorPos();
}

void Terminal::HandleNewLine()
{
    if (!line.empty())
    {
        bool found = false;
        for (const std::u32string& prevLine : lines)
        {
            if (line == prevLine)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            lines.push_back(line);
            lineIndex = lines.size();
        }
    }
    line.append(1, '\n');
    std::string inputLine = ToUtf8(line);
    line.clear();
    pos = 0;
    if (cursorPosY >= screenSizeY - 1)
    {
        Write(U"\n");
    }
    else
    {
        ++cursorPosY;
    }
    SetCursorPos(0, cursorPosY);
    outputEndCursorPosX = 0;
    outputEndCursorPosY = cursorPosY;
    for (char c : inputLine)
    {
        terminalInputBuffer.push_back(static_cast<uint8_t>(c));
    }
    Wakeup(terminalInputEvent);
}

void Terminal::HandleBackspace()
{
    if (pos > 0)
    {
        line = line.substr(0, pos - 1) + line.substr(pos);
        --pos;
        UpdateLine();
        UpdateCursorPos();
    }
}

void Terminal::HandleTab()
{
    // todo
}

void Terminal::HandleEof()
{
    eof = true;
}

void Terminal::HandleHome()
{
    pos = 0;
    UpdateCursorPos();
}

void Terminal::HandleEnd()
{
    pos = line.length();
    UpdateCursorPos();
}

void Terminal::HandleLeft()
{
    if (pos > 0)
    {
        --pos;
        UpdateCursorPos();
    }
}

void Terminal::HandleRight()
{
    if (pos < line.length())
    {
        ++pos;
        UpdateCursorPos();
    }
}

void Terminal::HandleUp()
{
    if (lineIndex > 0)
    {
        ClearUpdateLine();
        --lineIndex;
        if (lineIndex < lines.size())
        {
            line = lines[lineIndex];
        }
        UpdateLine();
        HandleEnd();
    }
}

void Terminal::HandleDown()
{
    if (lineIndex < int32_t(lines.size()) - 1)
    {
        ClearUpdateLine();
        ++lineIndex;
        line = lines[lineIndex];
        UpdateLine();
        HandleEnd();
    }
}

void Terminal::HandleControlLeft()
{
    if (pos > 0)
    {
        --pos;
        if (pos > 0)
        {
            char c = line[pos];
            while (c == ' ')
            {
                if (pos > 0)
                {
                    --pos;
                }
                if (pos > 0)
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
            while (c != ' ')
            {
                if (pos > 0)
                {
                    --pos;
                }
                if (pos > 0)
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
            if (c == ' ')
            {
                ++pos;
            }
        }
    }
    UpdateCursorPos();
}

void Terminal::HandleControlRight()
{
    if (pos < line.length())
    {
        ++pos;
        if (pos < line.length())
        {
            char c = line[pos];
            while (c != ' ')
            {
                if (pos < line.length())
                {
                    ++pos;
                }
                if (pos < line.length())
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
            while (c == ' ')
            {
                if (pos < line.length())
                {
                    ++pos;
                }
                if (pos < line.length())
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
        }
    }
    UpdateCursorPos();
}

void Terminal::HandleDel()
{
    if (pos < line.length())
    {
        ++pos;
        HandleBackspace();
    }
}

void Terminal::ClearUpdateLine()
{
    if (!echo) return;
    int32_t prevCursorPosX = cursorPosX;
    int32_t prevCursorPosY = cursorPosY;
    SetCursorPos(outputEndCursorPosX, outputEndCursorPosY);
    std::u32string updateLine;
    int32_t x = outputEndCursorPosX;
    int32_t y = outputEndCursorPosY;
    for (int i = 0; i < line.length(); ++i)
    {
        if (x + i < screenSizeX)
        {
            updateLine.append(1, ' ');
        }
        else
        {
            OsWriteConsole(consoleOutputHandle, updateLine.c_str());
            updateLine.clear();
            x = 0;
            ++y;
            updateLine.append(1, ' ');
        }
    }
    if (!updateLine.empty())
    {
        OsWriteConsole(consoleOutputHandle, updateLine.c_str());
        x += updateLine.length();
        updateLine.clear();
    }
    for (int i = x; i < screenSizeX; ++i)
    {
        updateLine.append(1, ' ');
    }
    if (!updateLine.empty())
    {
        OsWriteConsole(consoleOutputHandle, updateLine.c_str());
    }
    SetCursorPos(prevCursorPosX, prevCursorPosY);
}

void Terminal::UpdateLine()
{
    if (!echo) return;
    int32_t prevCursorPosX = cursorPosX;
    int32_t prevCursorPosY = cursorPosY;
    SetCursorPos(outputEndCursorPosX, outputEndCursorPosY);
    cursorPosX = outputEndCursorPosX;
    cursorPosY = outputEndCursorPosY;
    std::u32string updateLine;
    for (int i = 0; i < line.length(); ++i)
    {
        if (cursorPosX  < screenSizeX)
        {
            updateLine.append(1, line[i]);
            ++cursorPosX;
        }
        else
        {
            OsWriteConsole(consoleOutputHandle, updateLine.c_str());
            updateLine.clear();
            cursorPosX = 0;
            ++cursorPosY;
            updateLine.append(1, line[i]);
        }
    }
    if (!updateLine.empty())
    {
        OsWriteConsole(consoleOutputHandle, updateLine.c_str());
        updateLine.clear();
    }
    for (int i = cursorPosX; i < screenSizeX; ++i)
    {
        updateLine.append(1, ' ');
    }
    if (!updateLine.empty())
    {
        OsWriteConsole(consoleOutputHandle, updateLine.c_str());
    }
    SetCursorPos(prevCursorPosX, prevCursorPosY);
}

void Terminal::UpdateCursorPos()
{
    if (!echo) return;
    int32_t x = (outputEndCursorPosX + pos) % screenSizeX;
    int32_t y = outputEndCursorPosY + (outputEndCursorPosX + pos) / screenSizeX;
    if (y >= screenSizeY)
    {
        y = screenSizeY - 1;
    }
    SetCursorPos(x, y);
}

std::vector<uint8_t> Terminal::Read(int64_t count, cmsx::machine::Process* process)
{
    std::vector<uint8_t> bytes;
    while (true)
    {
        std::unique_lock<std::recursive_mutex> lock(machine->Lock());
        if (terminalInputBuffer.empty())
        {
            if (eof)
            {
                eof = false;
                return std::vector<uint8_t>();
            }
            sleepingOnTerminalInputEvent = true;
            Sleep(terminalInputEvent, process, lock);
            if (exiting || machine->Exiting()) return std::vector<uint8_t>();
            lock.lock();
            sleepingOnTerminalInputEvent = false;
            continue;
        }
        else
        {
            int64_t m = int64_t(terminalInputBuffer.size());
            if (mode == TerminalMode::cooked && m >= 3)
            {
                bool keyMsgRead = false;
                Utf8ToUtf32Engine readEngine;
                for (auto it = terminalInputBuffer.cbegin(); it != terminalInputBuffer.cend(); ++it)
                {
                    readEngine.Put(*it);
                    if (readEngine.ResulReady())
                    {
                        if (readEngine.Result() == keyMsg)
                        {
                            keyMsgRead = true;
                            break;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                if (keyMsgRead)
                {
                    if ((GetDebugMode() & debugTerminalMode) != 0)
                    {
                        DebugWrite("key.msg.read mode=cooked");
                    }
                    terminalInputBuffer.pop_front();
                    terminalInputBuffer.pop_front();
                    terminalInputBuffer.pop_front();
                    continue;
                }
            }
            int64_t n = std::min(m, count);
            for (int64_t i = 0; i < n; ++i)
            {
                uint8_t x = terminalInputBuffer.front();
                terminalInputBuffer.pop_front();
                bytes.push_back(x);
            }
            return bytes;
        }
    }
    return bytes;
}

void Terminal::Write(const std::u32string& utf32Chars)
{
    if (!utf32Chars.empty())
    {
        OsWriteConsole(consoleOutputHandle, utf32Chars.c_str());
        GetScreenBufferInfo();
    }
}

int64_t Terminal::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    std::u32string utf32Chars;
    for (uint8_t x : buffer)
    {
        utfEngine.Put(x);
        if (utfEngine.ResulReady())
        {
            char32_t ch = utfEngine.Result();
            if (ch < static_cast<char32_t>(0x100000u))
            {
                utf32Chars.append(1, ch);
            }
            else if (ch < 0x10FFFD)
            {
                uint8_t fgColor = static_cast<uint32_t>(ch) & 0xFF;
                uint8_t bgColor = (static_cast<uint32_t>(ch) >> 8) & 0xFF;
                uint16_t colorAttrs = static_cast<uint16_t>(fgColor) | (uint16_t(bgColor) << 4);
                Write(utf32Chars);
                utf32Chars.clear();
                OsSetConsoleTextAttribute(consoleOutputHandle, colorAttrs);
            }
            else if (ch == 0x10FFFD)
            {
                Write(utf32Chars);
                utf32Chars.clear();
                OsSetConsoleTextAttribute(consoleOutputHandle, defaultAttrs);
            }
        }
    }
    Write(utf32Chars);
    utf32Chars.clear();
    outputEndCursorPosX = cursorPosX;
    outputEndCursorPosY = cursorPosY;
    return buffer.size();
}

TerminalFile::TerminalFile() : File("TERMINAL")
{
}

std::vector<uint8_t> TerminalFile::Read(int64_t count, cmsx::machine::Process* process)
{
    return Terminal::Instance().Read(count, process);
}

int64_t TerminalFile::Write(const std::vector<uint8_t>& buffer, cmsx::machine::Process* process)
{
    return Terminal::Instance().Write(buffer, process);
}

int TerminalFile::Columns() const
{
    return Terminal::Instance().ScreenSizeX();
}

int TerminalFile::Rows() const
{
    return Terminal::Instance().ScreenSizeY();
}

int TerminalFile::CursorX() const
{
    return Terminal::Instance().CursorPosX();
}

int TerminalFile::CursorY() const
{
    return Terminal::Instance().CursorPosY();
}

void TerminalFile::SetCursorPos(int32_t cursorX, int32_t cursorY)
{
    Terminal::Instance().SetCursorPos(cursorX, cursorY);
}

void TerminalFile::SetCooked()
{
    Terminal::Instance().SetCooked();
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("cooked");
    }
}

void TerminalFile::SetRaw()
{
    Terminal::Instance().SetRaw();
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("raw");
    }
}

void TerminalFile::SetEcho(bool echo)
{
    Terminal::Instance().SetEcho(echo);
    if ((GetDebugMode() & debugTerminalMode) != 0)
    {
        DebugWrite("echo=" + std::to_string(echo));
    }
}

void TerminalFile::SendKey(char32_t key)
{
    Terminal::Instance().SendKey(key);
}

void TerminalFile::PushLines()
{
    Terminal::Instance().PushLines();
}

void TerminalFile::PopLines()
{
    Terminal::Instance().PopLines();
}

void SetTerminalMachine(cmsx::machine::Machine* machine)
{
    if (Terminal::Initialized())
    {
        Terminal::Instance().SetMachine(machine);
    }
}

void StartTerminal()
{
    if (Terminal::Initialized())
    {
        Terminal::Instance().Start();
    }
}

void StopTerminal()
{
    if (Terminal::Initialized())
    {
        Terminal::Instance().Stop();
    }
}

void InitTerminal()
{
    Terminal::Init();
}

void DoneTerminal()
{
    Terminal::Done();
}

} // namespace cmsx::kernel
