#include "Hooks.h"
#include "SDK.h"
#include "VMTHook.h"
#include "Globals.h"
#include "Math.h"
#include "headers/CapstoneDisassembler.hpp"
#include "headers/Detour/x86Detour.hpp"

#include "ImGui/imgui.h"
#include "ImGui/Directx9/imgui_impl_dx9.h"
#include "ImGui/imgui_internal.h"

#include "ImGuiRender.h"

std::unique_ptr<VMTHook> _d3d9_hook;
std::unique_ptr<VMTHook> pClientHook;
uint64_t processPacketTramp = NULL;
PLH::x86Detour *processPacket;
float lastTime = 0.0f;

static HRESULT __stdcall Reset( IDirect3DDevice9 *thisptr, D3DPRESENT_PARAMETERS *params );
static HRESULT __stdcall Present( IDirect3DDevice9 *thisptr, const RECT *src, const RECT *dest, HWND wnd_override, const RGNDATA *dirty_region );

typedef long( __stdcall *Reset_t ) ( IDirect3DDevice9 *, D3DPRESENT_PARAMETERS * );
typedef long( __stdcall *Present_t ) ( IDirect3DDevice9 *, const RECT *, const RECT *, HWND, const RGNDATA * );

std::vector<GamePacketID> ignorePackets = std::vector<GamePacketID> { GamePacketID::OnReplication };

NOINLINE int __fastcall h_ProcessPacket( UINT packetId, Packet *packet )
{
    int ret = PLH::FnCast( processPacketTramp, h_ProcessPacket )( packetId, packet );
    if ( std::find( ignorePackets.begin(), ignorePackets.end(), packetId ) == ignorePackets.end() )
        Globals::Log->LogInfo( "Received packet 0x%x func returned %d", packetId, ret );
    return ret;
}

NOINLINE bool __fastcall h_IsReplay( void *thisPtr )
{
    bool ret = pClientHook->GetOriginal<decltype( &h_IsReplay )>( 44 )( thisPtr );
    Globals::Log->LogInfo( "called isreplay, returned %d", ret );
    return ret;
}

auto Hooks::Init() -> void
{
    Globals::Log->LogInfo( "setting up hooks..." );

    //Globals::Log->LogInfo( "pClient is at 0x%x, deref vtable at 0x%x", g_Client, *( void ** )g_Client );

    _d3d9_hook = std::make_unique< VMTHook >( g_Renderer->DeviceHandler->Direct3DDevice9 );
    //pClientHook = std::make_unique<VMTHook>( g_Client );

    if ( SUCCEEDED( _d3d9_hook->Hook( 17, Present ) ) )
        Globals::Log->LogDebug( "hooked present" );
    else
        Globals::Log->LogError( "failed to hook present" );

    if ( SUCCEEDED( _d3d9_hook->Hook( 16, Reset ) ) )
        Globals::Log->LogDebug( "hooked reset" );
    else
        Globals::Log->LogError( "failed to hook reset" );

    //if ( SUCCEEDED( pClientHook->Hook( 44, h_IsReplay ) ) )
    //    Globals::Log->LogDebug( "hooked pClient isreplay" );
    //else
    //    Globals::Log->LogError( "failed pClient isreplay" );


    //PLH::CapstoneDisassembler mode( PLH::Mode::x86 );
    //processPacket = new PLH::x86Detour( ( char * )g_ProcessPacket, ( char * )&h_ProcessPacket, &processPacketTramp, mode );
    //processPacket->hook();
}

auto Hooks::Restore() -> void
{
    if ( _d3d9_hook != nullptr )
    {
        _d3d9_hook->Unhook( 17 );
        _d3d9_hook->Unhook( 16 );
    }

    if ( pClientHook != nullptr )
        pClientHook->Unhook( 44 );

    if ( processPacket != nullptr )
    {
        processPacket->unHook();
        delete processPacket;
    }
}

HRESULT STDMETHODCALLTYPE Present( IDirect3DDevice9 *thisptr, const RECT *src, const RECT *dest, HWND wnd_override, const RGNDATA *dirty_region )
{
    float curTime = ImGui::GetTime();
    const auto old_present = _d3d9_hook->GetOriginal< Present_t >( 17 );

    static auto init = false;

    if ( !init && thisptr )
    {
        ImGui_ImplDX9_Init( Globals::Input->GetMainWindow(), thisptr );

        init = true;
    }
    else if ( init && thisptr )
    {
        auto delta = curTime - lastTime;
        ImGui_ImplDX9_NewFrame();
        DrawImGui( delta );
        if ( Globals::Overlay )
            Globals::Overlay->Draw( delta );

        ImGui::PopStyleColor();
        ImGui::Render();
    }

    lastTime = curTime;

    return old_present( thisptr, src, dest, wnd_override, dirty_region );
}

HRESULT STDMETHODCALLTYPE Reset( IDirect3DDevice9 *thisptr, D3DPRESENT_PARAMETERS *params )
{
    const auto old_reset = _d3d9_hook->GetOriginal< Reset_t >( 16 );

    ImGui_ImplDX9_InvalidateDeviceObjects();

    auto hr = old_reset( thisptr, params );
    if ( SUCCEEDED( hr ) )
        ImGui_ImplDX9_CreateDeviceObjects();

    return hr;
}