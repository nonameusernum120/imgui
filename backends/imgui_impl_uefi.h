// dear imgui: Platform+Renderer Backend for UEFI/EDK2
// This needs to be used along with UEFI Graphics Output Protocol (GOP)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'UINT32' as texture identifier.
//  [X] Platform: Keyboard support using Simple Text Input Protocol.
//  [X] Platform: Mouse support using Simple Pointer Protocol.
//  [X] Renderer: Basic rendering using GOP Blt operations.
// Missing features:
//  [ ] Renderer: Large meshes support (64k+ vertices) with 16-bit indices.
//  [ ] Platform: Clipboard support (not available in UEFI).
//  [ ] Platform: Gamepad support.
//  [ ] Platform: Mouse cursor shape and visibility.

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

#ifndef IMGUI_DISABLE

// UEFI/EDK2 includes
#ifdef __cplusplus
extern "C" {
#endif

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimplePointer.h>

#ifdef __cplusplus
}
#endif

// Forward declarations
struct ImGui_ImplUefi_Data;

// Main API
IMGUI_IMPL_API bool     ImGui_ImplUefi_Init(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, EFI_SIMPLE_TEXT_INPUT_PROTOCOL* textInput, EFI_SIMPLE_POINTER_PROTOCOL* pointer = nullptr);
IMGUI_IMPL_API void     ImGui_ImplUefi_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplUefi_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplUefi_RenderDrawData(ImDrawData* draw_data);

// Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplUefi_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplUefi_DestroyDeviceObjects();

// Input handling
IMGUI_IMPL_API void     ImGui_ImplUefi_ProcessInputEvents();

// Texture management
IMGUI_IMPL_API UINT32   ImGui_ImplUefi_CreateTexture(const unsigned char* pixels, int width, int height);
IMGUI_IMPL_API void     ImGui_ImplUefi_DeleteTexture(UINT32 texture_id);

#endif // #ifndef IMGUI_DISABLE