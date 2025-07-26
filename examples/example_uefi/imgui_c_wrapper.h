/**
 * @file imgui_c_wrapper.h
 * @brief C wrapper for Dear ImGui functions
 * 
 * This header provides C-compatible wrappers for Dear ImGui C++ functions
 * to enable usage from C code in UEFI applications.
 */

#ifndef IMGUI_C_WRAPPER_H
#define IMGUI_C_WRAPPER_H

#include <Uefi.h>

#ifdef __cplusplus
extern "C" {
#endif

// Basic types
typedef struct ImVec2 { float x, y; } ImVec2;
typedef struct ImVec4 { float x, y, z, w; } ImVec4;
typedef void* ImTextureID;

// Window flags
typedef enum {
    ImGuiWindowFlags_None = 0,
    ImGuiWindowFlags_NoTitleBar = 1 << 0,
    ImGuiWindowFlags_NoResize = 1 << 1,
    ImGuiWindowFlags_NoMove = 1 << 2,
    ImGuiWindowFlags_NoScrollbar = 1 << 3,
    ImGuiWindowFlags_NoScrollWithMouse = 1 << 4,
    ImGuiWindowFlags_NoCollapse = 1 << 5,
    ImGuiWindowFlags_AlwaysAutoResize = 1 << 6,
    ImGuiWindowFlags_NoBackground = 1 << 7,
    ImGuiWindowFlags_NoSavedSettings = 1 << 8,
    ImGuiWindowFlags_NoMouseInputs = 1 << 9,
    ImGuiWindowFlags_MenuBar = 1 << 10,
    ImGuiWindowFlags_HorizontalScrollbar = 1 << 11,
    ImGuiWindowFlags_NoFocusOnAppearing = 1 << 12,
    ImGuiWindowFlags_NoBringToFrontOnFocus = 1 << 13,
    ImGuiWindowFlags_AlwaysVerticalScrollbar = 1 << 14,
    ImGuiWindowFlags_AlwaysHorizontalScrollbar = 1 << 15,
    ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,
    ImGuiWindowFlags_NoNavInputs = 1 << 18,
    ImGuiWindowFlags_NoNavFocus = 1 << 19,
    ImGuiWindowFlags_UnsavedDocument = 1 << 20,
    ImGuiWindowFlags_NoNav = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,
    ImGuiWindowFlags_NoDecoration = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse,
    ImGuiWindowFlags_NoInputs = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus
} ImGuiWindowFlags;

// Color edit flags  
typedef enum {
    ImGuiColorEditFlags_None = 0,
    ImGuiColorEditFlags_NoAlpha = 1 << 1,
    ImGuiColorEditFlags_NoPicker = 1 << 2,
    ImGuiColorEditFlags_NoOptions = 1 << 3,
    ImGuiColorEditFlags_NoSmallPreview = 1 << 4,
    ImGuiColorEditFlags_NoInputs = 1 << 5,
    ImGuiColorEditFlags_NoTooltip = 1 << 6,
    ImGuiColorEditFlags_NoLabel = 1 << 7,
    ImGuiColorEditFlags_NoSidePreview = 1 << 8,
    ImGuiColorEditFlags_NoDragDrop = 1 << 9,
    ImGuiColorEditFlags_NoBorder = 1 << 10
} ImGuiColorEditFlags;

// Slider flags
typedef enum {
    ImGuiSliderFlags_None = 0,
    ImGuiSliderFlags_AlwaysClamp = 1 << 4,
    ImGuiSliderFlags_Logarithmic = 1 << 5,
    ImGuiSliderFlags_NoRoundToFormat = 1 << 6,
    ImGuiSliderFlags_NoInput = 1 << 7
} ImGuiSliderFlags;

// C wrapper functions
BOOLEAN ImGui_CheckVersion(VOID);
VOID*   ImGui_CreateContext(VOID);
VOID    ImGui_DestroyContext(VOID);
VOID*   ImGui_GetIO(VOID);
VOID    ImGui_StyleColorsDark(VOID* dst);
VOID    ImGui_NewFrame(VOID);
VOID    ImGui_Render(VOID);
VOID*   ImGui_GetDrawData(VOID);

// Window functions
BOOLEAN ImGui_Begin(CONST CHAR8* name, BOOLEAN* p_open, INT32 flags);
VOID    ImGui_End(VOID);
BOOLEAN ImGui_BeginMainMenuBar(VOID);
VOID    ImGui_EndMainMenuBar(VOID);
BOOLEAN ImGui_BeginMenu(CONST CHAR8* label, BOOLEAN enabled);
VOID    ImGui_EndMenu(VOID);
BOOLEAN ImGui_MenuItem(CONST CHAR8* label, CONST CHAR8* shortcut, BOOLEAN selected, BOOLEAN enabled);
BOOLEAN ImGui_MenuItemPtr(CONST CHAR8* label, CONST CHAR8* shortcut, BOOLEAN* p_selected, BOOLEAN enabled);

// Widget functions
VOID    ImGui_Text(CONST CHAR8* fmt, ...);
VOID    ImGui_Separator(VOID);
BOOLEAN ImGui_Button(CONST CHAR8* label, ImVec2 size);
BOOLEAN ImGui_SliderFloat(CONST CHAR8* label, FLOAT* v, FLOAT v_min, FLOAT v_max, CONST CHAR8* format, INT32 flags);
BOOLEAN ImGui_ColorEdit3(CONST CHAR8* label, FLOAT col[3], INT32 flags);
VOID    ImGui_SameLine(FLOAT offset_from_start_x, FLOAT spacing);
VOID    ImGui_ShowDemoWindow(BOOLEAN* p_open);

// Constants
extern const ImVec2 ImVec2_zero;

#ifdef __cplusplus
}
#endif

#endif // IMGUI_C_WRAPPER_H