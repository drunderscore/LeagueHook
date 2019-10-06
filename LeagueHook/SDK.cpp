#include "SDK.h"
#include "Globals.h"
#include "Utilities.h"
#include "WinApi.h"

R3DRenderer *g_Renderer = nullptr;
GameStateInstance *g_GameStateInstance = nullptr;
void *g_ProcessPacket = nullptr;
void *g_Client = ( void * )0x32387B8;
void *g_GameTime = ( void * )0x0198D1F0;
GameObject *g_LocalPlayer = ( GameObject * )0x2977F48;

auto SDK::Init() -> void
{
    Globals::Log->LogInfo( "setting up sdk..." );

    auto main_module = Utils::GetModule( LI( GetCurrentProcessId )( ), FNV( "League of Legends.exe" ) );
    if ( main_module.modBaseAddr == nullptr )
    {
        Globals::Log->LogError( "SDK::Init() failed. could not get main module" );
        return;
    }

    g_Renderer = **reinterpret_cast<R3DRenderer ** *>( Utils::FindPattern( main_module, "A1 ? ? ? ? 83 B8 ? ? ? ? ? 75 50" ) + 0x1 );
    g_GameStateInstance = *reinterpret_cast<GameStateInstance **>( Utils::FindPattern( main_module, "83 3D ? ? ? ? ? 75 07 E8 ? ? ? ? 59 C3" ) + 0x2 );
    g_ProcessPacket = reinterpret_cast<void *>( Utils::FindPattern( main_module, "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 EC 53 56 57 50 8D 45 F4 64 A3 ? ? ? ? 8B DA 89 9D ? ? ? ?" ) );

#if _DEBUG
#define STRINGIFY_IMPL( s ) #s
#define STRINGIFY( s ) STRINGIFY_IMPL( s )
#define PRINT_INTERFACE( name ) Globals::Log->LogDebug( "interface: %s - 0x%p", STRINGIFY( name ), name )

    PRINT_INTERFACE( g_Renderer );
    PRINT_INTERFACE( g_GameStateInstance );
    PRINT_INTERFACE( g_ProcessPacket );
#endif
}