#include "SDK.h"
#include "DebugOverlay.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Math.h"
#include "Globals.h"

void DebugOverlay::Init()
{
    Globals::Overlay = new DebugOverlay();
}

void OverlayCircle::Draw( float delta, ImDrawList *drawList )
{
    auto *drawPos = new D3DXVECTOR3();
    if ( Math::WorldToScreen( D3DXVECTOR3( pos.x, pos.z, pos.y ), drawPos ) )
        drawList->AddCircle( ImVec2( drawPos->x, drawPos->y ), radius, IM_COL32( color.x, color.y, color.z, 255 ) );
}

void OverlayLine::Draw( float delta, ImDrawList *drawList )
{
    auto *drawPos1 = new D3DXVECTOR3();
    auto *drawPos2 = new D3DXVECTOR3();
    if ( Math::WorldToScreen( D3DXVECTOR3( pos1.x, pos1.z, pos1.y ), drawPos1 ) && Math::WorldToScreen( D3DXVECTOR3( pos2.x, pos2.z, pos2.y ), drawPos2 ) )
        drawList->AddLine( ImVec2( drawPos1->x, drawPos1->y ), ImVec2( drawPos2->x, drawPos2->y ), IM_COL32( color.x, color.y, color.z, 255 ), thick );
}

void DebugOverlay::AddCircle( Vector<float, 3> pos, Vector<unsigned char, 3> col, float radius, float lifeTime )
{
    std::lock_guard<std::mutex> guard( overlays_mutex );
    this->overlays.push_back( new OverlayCircle( pos, radius, col, lifeTime ) );
}

void DebugOverlay::AddLine( Vector<float, 3> pos1, Vector<float, 3> pos2, Vector<unsigned char, 3> col, float thick, float lifeTime )
{
    std::lock_guard<std::mutex> guard( overlays_mutex );
    this->overlays.push_back( new OverlayLine( pos1, pos2, col, thick, lifeTime ) );
}

void DebugOverlay::Draw( float delta )
{
    if ( ImGui::Begin( "##overlay", nullptr, ImVec2( 0.0f, 0.0f ), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs ) )
    {
        ImGui::SetWindowPos( ImVec2( 0.0f, 0.0f ), ImGuiSetCond_Always );
        ImGui::SetWindowSize( ImGui::GetIO().DisplaySize, ImGuiSetCond_Always );

        auto draw_list = ImGui::GetCurrentWindow()->DrawList;

        std::unique_lock<std::mutex> guard( overlays_mutex );
        auto it = overlays.begin();
        while ( it != overlays.end() )
        {
            auto o = *it;

            o->Draw( delta, draw_list );
            if ( ( o->curLifetime += delta ) >= o->maxLifetime )
            {
                it = overlays.erase( it );
                continue;
            }
            it++;
        }
        guard.unlock();

        draw_list->PushClipRectFullScreen();

        ImGui::End();
    }
}