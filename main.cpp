#include <windows.h>

#include "systemclass.h"

int CALLBACK WinMain( HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR     /*lpCmdLine*/, int       /*nCmdShow*/)
{
    SystemClass* system = nullptr;
    bool result = true;

    //Create the system object
    system = new SystemClass();
    if (!system) 
    {
        return 0;
    }

    //Initialize and run the system object
    result = system->Initialize();
    if (result)
    {
        system->Run();
    }

    //Shutdown and release the system object.
    system->Shutdown();
    delete system;
    system = 0;

    return 0;
}


