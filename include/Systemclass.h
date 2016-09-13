#pragma once

//--- Pre-processing Directives ---//
#define WIN32_LEAN_AND_MEAN

//--- Includes ---//
#include <windows.h>

//--- My Class Includes ---//
#include "inputclass.h"
#include "graphicsclass.h"

//Includes for FPS, CPU Usage, and Timers
#include "fpsclass.h"
#include "cpuclass.h"
#include "timerclass.h"

//--- SystemClass ---//
class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&);
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;

    InputClass* m_Input;
    GraphicsClass* m_Graphics;

    FpsClass* m_Fps;
    CpuClass* m_Cpu;
    TimerClass* m_Timer;
};

//--- Function Prototypes ---//
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//--- Globals ---//
static SystemClass* ApplicationHandle = 0;


