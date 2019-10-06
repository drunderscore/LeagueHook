#include "SDK.h"
#include "Globals.h"
#include "Math.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "imgui_memory_editor.h"

class OverlayCircle
{
public:
    float x, y, z, radius, lifeTime, decay;
    OverlayCircle( float x, float y, float z, float radius, float lifeTime ) : x( x ), y( y ), z( z ), radius( radius ), lifeTime( lifeTime ), decay( 0.0f ) {}
};

class OverlayLine
{
public:
    float x1, y1, x2, y2;
    OverlayLine( float x1, float y1, float x2, float y2 ) : x1( x1 ), y1( y1 ), x2( x2 ), y2( y2 ) {}
};

std::vector<OverlayCircle> circles;
std::vector<OverlayLine> lines;

void AddCircle( float x, float y, float z, float radius, float lifeTime )
{
    circles.push_back( OverlayCircle( x, y, z, radius, lifeTime ) );
}

void AddLine( float x1, float y1, float x2, float y2 )
{
    lines.push_back( OverlayLine( x1, y1, x2, y2 ) );
}

void DrawImGui( float delta )
{
    static auto draw_hovered_obj = false;
    static auto draw_hovered_obj_name = false;
    static bool drawSelfPosition;
    static bool drawMemoryEditor;
    static MemoryEditor editor;

    if ( Globals::ShowMenu )
    {
        if ( ImGui::Begin( "Debug stuffs", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
        {
            ImGui::Text( "Time: %.4f", ImGui::GetTime() );
            ImGui::Text( "Delta: %.4f", delta );
            ImGui::Checkbox( "Draw Hovered Object", &draw_hovered_obj );

            if ( draw_hovered_obj )
                ImGui::Checkbox( "- Name", &draw_hovered_obj_name );

            ImGui::Checkbox( "Draw Self Position", &drawSelfPosition );
            ImGui::Checkbox( "Draw Memory Editor", &drawMemoryEditor );
            if ( drawSelfPosition )
            {
                D3DXVECTOR3 world = g_LocalPlayer->WorldPosition;
                ImGui::Text( "- %.2f, %.2f, %.2f", world.x, world.y, world.z );
            }
        }

        if ( drawMemoryEditor )
            editor.DrawWindow( "Memory", GetModuleHandle( NULL ), 0xFFFFFFFF );

        if ( ImGui::CollapsingHeader( "Unstable Values" ) )
            ImGui::Text( "obj manager max?: %d", *( int * )0x32228C4 );
        ImGui::End();
    }

    ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f ) );

    if ( ImGui::Begin( "##overlay", nullptr, ImVec2( 0.0f, 0.0f ), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs ) )
    {
        ImGui::SetWindowPos( ImVec2( 0.0f, 0.0f ), ImGuiSetCond_Always );
        ImGui::SetWindowSize( ImGui::GetIO().DisplaySize, ImGuiSetCond_Always );

        auto draw_list = ImGui::GetCurrentWindow()->DrawList;

        auto c = circles.begin();
        while ( c != circles.end() )
        {
            c->decay += delta;
            if ( c->decay >= c->lifeTime )
            {
                c = circles.erase( c );
                continue;
            }

            auto *pos = new D3DXVECTOR3();
            if ( Math::WorldToScreen( D3DXVECTOR3( c->x, c->y, c->z ), pos ) )
                draw_list->AddCircle( ImVec2( pos->x, pos->y ), c->radius, ImColor( 255, 0, 0, 255 ) );

            c++;
        }

        if ( draw_hovered_obj )
        {
            auto hovered_obj = g_GameStateInstance->HoveredObject;
            if ( hovered_obj != nullptr )
            {
                auto *cursor_w2s = new D3DXVECTOR3();
                if ( Math::WorldToScreen( hovered_obj->WorldPosition, cursor_w2s ) )
                {
                    draw_list->AddCircle( ImVec2( cursor_w2s->x, cursor_w2s->y ), 25, ImColor( 255, 5, 100, 255 ), 100, 2 );

                    if ( draw_hovered_obj_name )
                        draw_list->AddText( ImVec2( cursor_w2s->x + 35, cursor_w2s->y - 5 ), ImColor( 255, 255, 100, 255 ), hovered_obj->ObjectName );
                }
            }
        }

        if ( drawSelfPosition )
        {
            auto *pos = new D3DXVECTOR3();
            if ( Math::WorldToScreen( g_LocalPlayer->WorldPosition, pos ) )
                draw_list->AddCircle( ImVec2( pos->x, pos->y ), 5, ImColor( 0, 255, 0, 255 ) );

        }

        draw_list->PushClipRectFullScreen();

        ImGui::End();
    }
}