#include "SDK.h"
#include "Globals.h"
#include "Math.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "imgui_memory_editor.h"

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
        }
        ImGui::End();
    }

    ImGui::PushStyleColor( ImGuiCol_WindowBg, ImVec4( 0.0f, 0.0f, 0.0f, 0.0f ) );
}