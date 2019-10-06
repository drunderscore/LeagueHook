#include "NetworkDebug.h" // prevent windows.h before winsock2.h!

#include "Globals.h"
#include "SDK.h"
#include "Hooks.h"
#include "InputManager.h"
#include "WinApi.h"


#include <windows.h>
#include <thread>

static auto WINAPI OnAttach() -> void
{
    Globals::Log = new Logger();
    Globals::Log->LogInfo( "initializing..." );

    SDK::Init();

    Globals::Input = new InputManager( g_Renderer->DeviceHandler->Direct3DDevice9 );
    Globals::Input->RegisterHotkey( VK_DELETE, []() { Globals::ShowMenu = !Globals::ShowMenu; } );

    Hooks::Init();
    DebugOverlay::Init();
    NetworkDebug::Init(); // should be on another thread...

    Globals::Log->LogInfo( "finished initializing!" );
}

static auto WINAPI OnDetach() -> void
{
    delete Globals::Log;
    delete Globals::Input;
    delete Globals::NetDebug;
    delete Globals::Overlay;
}

auto WINAPI DllMain( HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved ) -> BOOL
{
    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
            if ( hinstDll )
                LI( DisableThreadLibraryCalls )( hinstDll );
            LI( CreateThread )( nullptr, 0, LPTHREAD_START_ROUTINE( OnAttach ), hinstDll, 0, nullptr );
            break;
        case DLL_PROCESS_DETACH:
            if ( !lpReserved )
            {
                Hooks::Restore();
                if ( Globals::NetDebug != nullptr )
                    Globals::NetDebug->Close();
            }
            OnDetach();
            break;
        default:
            break;
    }

    return TRUE;
}