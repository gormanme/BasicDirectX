#include "systemclass.h"

SystemClass::SystemClass()
{
    m_Input = 0;
    m_Graphics = 0;
}


SystemClass::SystemClass(const SystemClass& /*other*/)
{

}

SystemClass::~SystemClass()
{

}


//--- Initialize(): Does all necessary setup for the application. ---//
bool SystemClass::Initialize()
{
    int screenWidth = 0;
    int screenHeight = 0;
    bool result = true;

    //Initialize the Windows API
    InitializeWindows(screenWidth, screenHeight);

    //Create the input object. This object will be used to handle reading the keyboard input from the user.
    m_Input = new InputClass();
    if(!m_Input)
    {
        return false;
    }

    //Initialize the input object
    m_Input->Initialize();

    //Create the graphics object. This object will handle rendering all the graphcis for this application.
    m_Graphics = new GraphicsClass();
    if (!m_Graphics)
    {
        return false;
    }

    //Initialize the graphics object
    result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
    if (!result)
    {
        return false;
    }

    return true;

}


//--- Shutdown(): Does all necessary cleanup/releasing for the graphcis and input objects. Also shuts down window and handles. ---//
void SystemClass::Shutdown()
{
    //Release the graphics object
    if (m_Graphics)
    {
        m_Graphics->Shutdown();
        delete m_Graphics;
        m_Graphics = 0;
    }

    //Release the input object
    if (m_Input)
    {
        delete m_Input;
        m_Input = 0;
    }

    //Shutdown the window
    ShutdownWindows();

}


//--- Run(): The main loop where the application runs. ---//
void SystemClass::Run()
{
    MSG msg = {0}; //This just initializes this type of variable to 0.
    bool done = false;
    bool result = true;

    //Loop until there is a quit message from the window or the user.
    while (!done)
    {
        //Handle the windows messages
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //If Winodws signals to end the application then exit out.
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            //Otherwise do the frame processing
            result = Frame();
            if (!result)
            {
                done = true;
            }
        }
    }
}


//--- Frame(): Check to see if the user has pressed escape and wants to quit. If not, then do frame processing and render graphics for that frame. ---//
bool SystemClass::Frame()
{
    bool result = true;

    //Check if the user pressed escape and wants to exit the application
    if (m_Input->IsKeyDown(VK_ESCAPE))
    {
        return false;
    }

    //Do the frame processing for the graphics object
    result = m_Graphics->Frame();
    if (!result)
    {
        return false;
    }

    return true;
}


//--- MessageHandler(): Listens for certain Windows system messages. In this case, if a key is pressed. ---//
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        //Check if a key has been pressed on the keyboard
        case WM_KEYDOWN:
        {
            //If a key is pressed send it to the input object so it can record that state
            m_Input->KeyDown((unsigned int)wparam);
            return 0;
        }
        
        //Check if a key has been released on the keyboard
        case WM_KEYUP:
        {
            //If a key is released then send it to the input object so it can unset the state for that key
            m_Input->KeyUp((unsigned int)wparam);
            return 0;
        }
        
        //Any other messages send to the default message handler as our application won't make use of them
        default:
        {
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }
}


//--- InitializeWindow(): Builds the window to render to. ---//
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
    WNDCLASSEX wc = {0};
    DEVMODE dmScreenSettings = {0};
    int posX = 0;
    int posY = 0;

    // Get an external pointer to this object.
    ApplicationHandle = this;

    // Get the instance of this application
    m_hinstance = GetModuleHandle(NULL);

    // Give the application a name.
    m_applicationName = L"Engine";

    // Setup the windows class with default settings
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = m_applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class
    RegisterClassEx(&wc);

	// If windowed then set it to 800x600 resolution.
	screenWidth = 800;
	screenHeight = 600;

	// Place the window in the middle of the screen.
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    // Create the window with the screen settings and get the handle to it
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
        WS_OVERLAPPEDWINDOW,//WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
        posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

    // Bring the window up on the screen and set it as main focus
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    // Hide the mouse cursor
    ShowCursor(false);

    return;
}


//--- ShutdownWindows(): Returns the screen settings back to normal and releases the window and handles. ---//
void SystemClass::ShutdownWindows()
{
    //Show the mouse cursor
    ShowCursor(true);

    //Remove the window
    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    //Remove the application instance
    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = NULL;

    //Release the pointer to this class
    ApplicationHandle = NULL;

}


//--- WndProc(): Where Windows sends it's messages. ---//
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
        //Check if the window is being destroyed
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        //Check if the window is being closed
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }

        //All other messages pass to the message handler in the system class
        default:
        {
            return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
        }
    }
}


