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
float lastTime = 0.0f;

static HRESULT __stdcall Reset( IDirect3DDevice9 *thisptr, D3DPRESENT_PARAMETERS *params );
static HRESULT __stdcall Present( IDirect3DDevice9 *thisptr, const RECT *src, const RECT *dest, HWND wnd_override, const RGNDATA *dirty_region );

typedef long( __stdcall *Reset_t ) ( IDirect3DDevice9 *, D3DPRESENT_PARAMETERS * );
typedef long( __stdcall *Present_t ) ( IDirect3DDevice9 *, const RECT *, const RECT *, HWND, const RGNDATA * );

auto Hooks::Init() -> void
{
    Globals::Log->LogInfo( "setting up hooks..." );

    _d3d9_hook = std::make_unique< VMTHook >( g_Renderer->DeviceHandler->Direct3DDevice9 );

    if ( SUCCEEDED( _d3d9_hook->Hook( 17, Present ) ) )
        Globals::Log->LogDebug( "hooked present" );
    else
        Globals::Log->LogError( "failed to hook present" );

    if ( SUCCEEDED( _d3d9_hook->Hook( 16, Reset ) ) )
        Globals::Log->LogDebug( "hooked reset" );
    else
        Globals::Log->LogError( "failed to hook reset" );

}

auto Hooks::Restore() -> void
{
    if ( _d3d9_hook != nullptr )
    {
        _d3d9_hook->Unhook( 17 );
        _d3d9_hook->Unhook( 16 );
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