/**
 * @file imgui_c_wrapper.cpp
 * @brief Implementation of C wrapper for Dear ImGui functions
 */

#include "imgui_c_wrapper.h"
#include "../../imgui.h"
#include <stdarg.h>
#include <stdio.h>

extern "C" {

// Constants
const ImVec2 ImVec2_zero = {0.0f, 0.0f};

// Core functions
BOOLEAN ImGui_CheckVersion(VOID)
{
    IMGUI_CHECKVERSION();
    return TRUE;
}

VOID* ImGui_CreateContext(VOID)
{
    return ImGui::CreateContext();
}

VOID ImGui_DestroyContext(VOID)
{
    ImGui::DestroyContext();
}

VOID* ImGui_GetIO(VOID)
{
    return &ImGui::GetIO();
}

VOID ImGui_StyleColorsDark(VOID* dst)
{
    ImGui::StyleColorsDark((ImGuiStyle*)dst);
}

VOID ImGui_NewFrame(VOID)
{
    ImGui::NewFrame();
}

VOID ImGui_Render(VOID)
{
    ImGui::Render();
}

VOID* ImGui_GetDrawData(VOID)
{
    return ImGui::GetDrawData();
}

// Window functions
BOOLEAN ImGui_Begin(CONST CHAR8* name, BOOLEAN* p_open, INT32 flags)
{
    bool* cpp_open = nullptr;
    bool cpp_open_val = false;
    
    if (p_open != nullptr) {
        cpp_open_val = (*p_open != FALSE);
        cpp_open = &cpp_open_val;
    }
    
    bool result = ImGui::Begin(name, cpp_open, flags);
    
    if (p_open != nullptr) {
        *p_open = cpp_open_val ? TRUE : FALSE;
    }
    
    return result ? TRUE : FALSE;
}

VOID ImGui_End(VOID)
{
    ImGui::End();
}

BOOLEAN ImGui_BeginMainMenuBar(VOID)
{
    return ImGui::BeginMainMenuBar() ? TRUE : FALSE;
}

VOID ImGui_EndMainMenuBar(VOID)
{
    ImGui::EndMainMenuBar();
}

BOOLEAN ImGui_BeginMenu(CONST CHAR8* label, BOOLEAN enabled)
{
    return ImGui::BeginMenu(label, enabled != FALSE) ? TRUE : FALSE;
}

VOID ImGui_EndMenu(VOID)
{
    ImGui::EndMenu();
}

BOOLEAN ImGui_MenuItem(CONST CHAR8* label, CONST CHAR8* shortcut, BOOLEAN selected, BOOLEAN enabled)
{
    return ImGui::MenuItem(label, shortcut, selected != FALSE, enabled != FALSE) ? TRUE : FALSE;
}

BOOLEAN ImGui_MenuItemPtr(CONST CHAR8* label, CONST CHAR8* shortcut, BOOLEAN* p_selected, BOOLEAN enabled)
{
    bool cpp_selected = false;
    if (p_selected != nullptr) {
        cpp_selected = (*p_selected != FALSE);
    }
    
    bool result = ImGui::MenuItem(label, shortcut, p_selected ? &cpp_selected : nullptr, enabled != FALSE);
    
    if (p_selected != nullptr) {
        *p_selected = cpp_selected ? TRUE : FALSE;
    }
    
    return result ? TRUE : FALSE;
}

// Widget functions
VOID ImGui_Text(CONST CHAR8* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    buffer[sizeof(buffer) - 1] = '\0';
    
    ImGui::Text("%s", buffer);
    
    va_end(args);
}

VOID ImGui_Separator(VOID)
{
    ImGui::Separator();
}

BOOLEAN ImGui_Button(CONST CHAR8* label, ImVec2 size)
{
    ImVec2 cpp_size(size.x, size.y);
    return ImGui::Button(label, cpp_size) ? TRUE : FALSE;
}

BOOLEAN ImGui_SliderFloat(CONST CHAR8* label, FLOAT* v, FLOAT v_min, FLOAT v_max, CONST CHAR8* format, INT32 flags)
{
    return ImGui::SliderFloat(label, v, v_min, v_max, format, flags) ? TRUE : FALSE;
}

BOOLEAN ImGui_ColorEdit3(CONST CHAR8* label, FLOAT col[3], INT32 flags)
{
    return ImGui::ColorEdit3(label, col, flags) ? TRUE : FALSE;
}

VOID ImGui_SameLine(FLOAT offset_from_start_x, FLOAT spacing)
{
    ImGui::SameLine(offset_from_start_x, spacing);
}

VOID ImGui_ShowDemoWindow(BOOLEAN* p_open)
{
    bool cpp_open = true;
    if (p_open != nullptr) {
        cpp_open = (*p_open != FALSE);
    }
    
    ImGui::ShowDemoWindow(p_open ? &cpp_open : nullptr);
    
    if (p_open != nullptr) {
        *p_open = cpp_open ? TRUE : FALSE;
    }
}

} // extern "C"