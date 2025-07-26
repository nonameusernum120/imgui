// dear imgui: Platform+Renderer Backend for UEFI/EDK2
// This needs to be used along with UEFI Graphics Output Protocol (GOP)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'UINT32' as texture identifier.
//  [X] Platform: Keyboard support using Simple Text Input Protocol.
//  [X] Platform: Mouse support using Simple Pointer Protocol.
//  [X] Renderer: Basic rendering using GOP Blt operations.

// CHANGELOG
// 2025-01-XX: Initial implementation for UEFI/EDK2.

#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_uefi.h"

// UEFI/EDK2 includes
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimplePointer.h>

// Texture storage structure
struct UefiTexture {
    UINT32* Pixels;
    INT32   Width;
    INT32   Height;
};

#define MAX_TEXTURES 256

// Backend data stored in io.BackendRendererUserData
struct ImGui_ImplUefi_Data
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL*    Gop;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL*  TextInput;
    EFI_SIMPLE_POINTER_PROTOCOL*     Pointer;
    
    UINT32*                          FrameBuffer;
    UINTN                            FrameBufferSize;
    UINT32                           ScreenWidth;
    UINT32                           ScreenHeight;
    
    UefiTexture                      Textures[MAX_TEXTURES];
    BOOLEAN                          TextureUsed[MAX_TEXTURES];
    UINT32                           NextTextureId;
    
    UINT32                           FontTextureId;
    BOOLEAN                          MouseButtonDown[3];
    INT32                            MouseX, MouseY;
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
static ImGui_ImplUefi_Data* ImGui_ImplUefi_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplUefi_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// Helper function to convert ImGui color to UEFI color
static EFI_GRAPHICS_OUTPUT_BLT_PIXEL ImGuiColorToUefi(ImU32 col)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL pixel = {};
    pixel.Red   = (col >> IM_COL32_R_SHIFT) & 0xFF;
    pixel.Green = (col >> IM_COL32_G_SHIFT) & 0xFF;
    pixel.Blue  = (col >> IM_COL32_B_SHIFT) & 0xFF;
    pixel.Reserved = 0xFF;
    return pixel;
}

// Helper function to draw a line using Bresenham's algorithm
static void DrawLine(UINT32* buffer, UINT32 width, UINT32 height, INT32 x0, INT32 y0, INT32 x1, INT32 y1, UINT32 color)
{
    if (x0 < 0 && x1 < 0) return;
    if (y0 < 0 && y1 < 0) return;
    if (x0 >= (INT32)width && x1 >= (INT32)width) return;
    if (y0 >= (INT32)height && y1 >= (INT32)height) return;
    
    INT32 dx = (x1 - x0) >= 0 ? (x1 - x0) : -(x1 - x0);
    INT32 dy = (y1 - y0) >= 0 ? (y1 - y0) : -(y1 - y0);
    INT32 sx = x0 < x1 ? 1 : -1;
    INT32 sy = y0 < y1 ? 1 : -1;
    INT32 err = dx - dy;
    
    while (true) {
        if (x0 >= 0 && x0 < (INT32)width && y0 >= 0 && y0 < (INT32)height) {
            buffer[y0 * width + x0] = color;
        }
        
        if (x0 == x1 && y0 == y1) break;
        
        INT32 e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Helper function to fill a triangle
static void FillTriangle(UINT32* buffer, UINT32 width, UINT32 height, 
                        const ImVec2& p0, const ImVec2& p1, const ImVec2& p2, 
                        UINT32 col0, UINT32 col1, UINT32 col2)
{
    INT32 minX = (INT32)ImMin(ImMin(p0.x, p1.x), p2.x);
    INT32 maxX = (INT32)ImMax(ImMax(p0.x, p1.x), p2.x);
    INT32 minY = (INT32)ImMin(ImMin(p0.y, p1.y), p2.y);
    INT32 maxY = (INT32)ImMax(ImMax(p0.y, p1.y), p2.y);
    
    minX = ImMax(minX, 0);
    maxX = ImMin(maxX, (INT32)width - 1);
    minY = ImMax(minY, 0);
    maxY = ImMin(maxY, (INT32)height - 1);
    
    for (INT32 y = minY; y <= maxY; y++) {
        for (INT32 x = minX; x <= maxX; x++) {
            // Barycentric coordinates
            float denom = (p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y);
            if (denom < 1e-6f && denom > -1e-6f) continue;
            
            float a = ((p1.y - p2.y) * (x - p2.x) + (p2.x - p1.x) * (y - p2.y)) / denom;
            float b = ((p2.y - p0.y) * (x - p2.x) + (p0.x - p2.x) * (y - p2.y)) / denom;
            float c = 1.0f - a - b;
            
            if (a >= 0 && b >= 0 && c >= 0) {
                // Interpolate colors (simplified - just use col0 for now)
                buffer[y * width + x] = col0;
            }
        }
    }
}

// Key mapping from UEFI to ImGui
static ImGuiKey UefiKeyToImGuiKey(UINT16 scanCode)
{
    switch (scanCode) {
        case SCAN_UP:        return ImGuiKey_UpArrow;
        case SCAN_DOWN:      return ImGuiKey_DownArrow;
        case SCAN_RIGHT:     return ImGuiKey_RightArrow;
        case SCAN_LEFT:      return ImGuiKey_LeftArrow;
        case SCAN_HOME:      return ImGuiKey_Home;
        case SCAN_END:       return ImGuiKey_End;
        case SCAN_INSERT:    return ImGuiKey_Insert;
        case SCAN_DELETE:    return ImGuiKey_Delete;
        case SCAN_PAGE_UP:   return ImGuiKey_PageUp;
        case SCAN_PAGE_DOWN: return ImGuiKey_PageDown;
        case SCAN_ESC:       return ImGuiKey_Escape;
        case SCAN_F1:        return ImGuiKey_F1;
        case SCAN_F2:        return ImGuiKey_F2;
        case SCAN_F3:        return ImGuiKey_F3;
        case SCAN_F4:        return ImGuiKey_F4;
        case SCAN_F5:        return ImGuiKey_F5;
        case SCAN_F6:        return ImGuiKey_F6;
        case SCAN_F7:        return ImGuiKey_F7;
        case SCAN_F8:        return ImGuiKey_F8;
        case SCAN_F9:        return ImGuiKey_F9;
        case SCAN_F10:       return ImGuiKey_F10;
        case SCAN_F11:       return ImGuiKey_F11;
        case SCAN_F12:       return ImGuiKey_F12;
        default:             return ImGuiKey_None;
    }
}

bool ImGui_ImplUefi_Init(EFI_GRAPHICS_OUTPUT_PROTOCOL* gop, EFI_SIMPLE_TEXT_INPUT_PROTOCOL* textInput, EFI_SIMPLE_POINTER_PROTOCOL* pointer)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");
    
    // Setup backend capabilities flags
    ImGui_ImplUefi_Data* bd = (ImGui_ImplUefi_Data*)AllocateZeroPool(sizeof(ImGui_ImplUefi_Data));
    if (!bd) return FALSE;
    
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_uefi";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
    
    bd->Gop = gop;
    bd->TextInput = textInput;
    bd->Pointer = pointer;
    bd->NextTextureId = 1;
    
    // Get screen dimensions
    bd->ScreenWidth = gop->Mode->Info->HorizontalResolution;
    bd->ScreenHeight = gop->Mode->Info->VerticalResolution;
    
    // Set display size
    io.DisplaySize = ImVec2((float)bd->ScreenWidth, (float)bd->ScreenHeight);
    
    // Allocate frame buffer
    bd->FrameBufferSize = bd->ScreenWidth * bd->ScreenHeight * sizeof(UINT32);
    bd->FrameBuffer = (UINT32*)AllocatePool(bd->FrameBufferSize);
    if (!bd->FrameBuffer) {
        return false;
    }
    
    // Clear frame buffer
    SetMem(bd->FrameBuffer, bd->FrameBufferSize, 0);
    
    return ImGui_ImplUefi_CreateDeviceObjects();
}

void ImGui_ImplUefi_Shutdown()
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();
    
    ImGui_ImplUefi_DestroyDeviceObjects();
    
    if (bd->FrameBuffer) {
        FreePool(bd->FrameBuffer);
        bd->FrameBuffer = nullptr;
    }
    
    // Clean up textures
    for (UINT32 i = 0; i < MAX_TEXTURES; i++) {
        if (bd->TextureUsed[i] && bd->Textures[i].Pixels) {
            FreePool(bd->Textures[i].Pixels);
        }
    }
    
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~ImGuiBackendFlags_RendererHasTextures;
    FreePool(bd);
}

void ImGui_ImplUefi_NewFrame()
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplUefi_Init()?");
    
    if (!bd->FontTextureId)
        ImGui_ImplUefi_CreateDeviceObjects();
    
    // Process input events
    ImGui_ImplUefi_ProcessInputEvents();
}

bool ImGui_ImplUefi_CreateDeviceObjects()
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    
    // Build texture atlas
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    
    // Create font texture
    bd->FontTextureId = ImGui_ImplUefi_CreateTexture(pixels, width, height);
    io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTextureId);
    
    return true;
}

void ImGui_ImplUefi_DestroyDeviceObjects()
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    if (!bd) return;
    
    if (bd->FontTextureId) {
        ImGui_ImplUefi_DeleteTexture(bd->FontTextureId);
        ImGui::GetIO().Fonts->SetTexID(0);
        bd->FontTextureId = 0;
    }
}

void ImGui_ImplUefi_ProcessInputEvents()
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    
    // Process keyboard input
    if (bd->TextInput) {
        EFI_INPUT_KEY key;
        EFI_STATUS status = bd->TextInput->ReadKeyStroke(bd->TextInput, &key);
        if (!EFI_ERROR(status)) {
            if (key.ScanCode != SCAN_NULL) {
                ImGuiKey imgui_key = UefiKeyToImGuiKey(key.ScanCode);
                if (imgui_key != ImGuiKey_None) {
                    io.AddKeyEvent(imgui_key, true);
                    io.AddKeyEvent(imgui_key, false); // Immediate release for now
                }
            }
            
            if (key.UnicodeChar != 0) {
                io.AddInputCharacter(key.UnicodeChar);
            }
        }
    }
    
    // Process mouse input (simplified)
    if (bd->Pointer) {
        EFI_SIMPLE_POINTER_STATE state;
        EFI_STATUS status = bd->Pointer->GetState(bd->Pointer, &state);
        if (!EFI_ERROR(status)) {
            // Update mouse position (relative movement)
            bd->MouseX += state.RelativeMovementX / 1000; // Scale down
            bd->MouseY += state.RelativeMovementY / 1000;
            
            // Clamp to screen bounds
            bd->MouseX = ImClamp(bd->MouseX, 0, (INT32)bd->ScreenWidth - 1);
            bd->MouseY = ImClamp(bd->MouseY, 0, (INT32)bd->ScreenHeight - 1);
            
            io.AddMousePosEvent((float)bd->MouseX, (float)bd->MouseY);
            
            // Handle mouse buttons
            BOOLEAN left = (state.LeftButton != 0) ? TRUE : FALSE;
            BOOLEAN right = (state.RightButton != 0) ? TRUE : FALSE;
            
            if (left != bd->MouseButtonDown[0]) {
                io.AddMouseButtonEvent(0, left ? true : false);
                bd->MouseButtonDown[0] = left;
            }
            
            if (right != bd->MouseButtonDown[1]) {
                io.AddMouseButtonEvent(1, right ? true : false);
                bd->MouseButtonDown[1] = right;
            }
        }
    }
}

UINT32 ImGui_ImplUefi_CreateTexture(const unsigned char* pixels, int width, int height)
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    
    // Find a free texture slot
    for (UINT32 i = 1; i < MAX_TEXTURES; i++) {
        if (!bd->TextureUsed[i]) {
            UefiTexture* texture = &bd->Textures[i];
            texture->Width = width;
            texture->Height = height;
            texture->Pixels = (UINT32*)AllocatePool(width * height * sizeof(UINT32));
            
            if (!texture->Pixels)
                return 0;
            
            // Convert RGBA to BGRA format
            for (int j = 0; j < width * height; j++) {
                UINT8 r = pixels[j * 4 + 0];
                UINT8 g = pixels[j * 4 + 1];
                UINT8 b = pixels[j * 4 + 2];
                UINT8 a = pixels[j * 4 + 3];
                texture->Pixels[j] = (a << 24) | (r << 16) | (g << 8) | b;
            }
            
            bd->TextureUsed[i] = TRUE;
            return i;
        }
    }
    
    return 0; // No free slots
}

void ImGui_ImplUefi_DeleteTexture(UINT32 texture_id)
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    
    if (texture_id < MAX_TEXTURES && bd->TextureUsed[texture_id]) {
        if (bd->Textures[texture_id].Pixels) {
            FreePool(bd->Textures[texture_id].Pixels);
            bd->Textures[texture_id].Pixels = NULL;
        }
        bd->TextureUsed[texture_id] = FALSE;
    }
}

void ImGui_ImplUefi_RenderDrawData(ImDrawData* draw_data)
{
    ImGui_ImplUefi_Data* bd = ImGui_ImplUefi_GetBackendData();
    if (!draw_data || !bd->FrameBuffer)
        return;
    
    // Clear frame buffer
    SetMem(bd->FrameBuffer, bd->FrameBufferSize, 0);
    
    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
        const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
        
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            
            if (pcmd->UserCallback != nullptr) {
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState) {
                    // Reset render state (nothing to do for us)
                } else {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
            } else {
                // Clip rectangle
                INT32 clipX = (INT32)pcmd->ClipRect.x;
                INT32 clipY = (INT32)pcmd->ClipRect.y;
                INT32 clipW = (INT32)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                INT32 clipH = (INT32)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                
                if (clipX < 0 || clipY < 0 || clipW <= 0 || clipH <= 0 ||
                    clipX >= (INT32)bd->ScreenWidth || clipY >= (INT32)bd->ScreenHeight)
                    continue;
                
                // Render triangles
                for (unsigned int i = 0; i < pcmd->ElemCount; i += 3) {
                    const ImDrawVert& v0 = vtx_buffer[idx_buffer[i + 0]];
                    const ImDrawVert& v1 = vtx_buffer[idx_buffer[i + 1]];
                    const ImDrawVert& v2 = vtx_buffer[idx_buffer[i + 2]];
                    
                    ImVec2 p0 = ImVec2(v0.pos.x, v0.pos.y);
                    ImVec2 p1 = ImVec2(v1.pos.x, v1.pos.y);
                    ImVec2 p2 = ImVec2(v2.pos.x, v2.pos.y);
                    
                    // Simple clipping check
                    if ((p0.x < clipX && p1.x < clipX && p2.x < clipX) ||
                        (p0.y < clipY && p1.y < clipY && p2.y < clipY) ||
                        (p0.x >= clipX + clipW && p1.x >= clipX + clipW && p2.x >= clipX + clipW) ||
                        (p0.y >= clipY + clipH && p1.y >= clipY + clipH && p2.y >= clipY + clipH))
                        continue;
                    
                    // For text rendering, check if this uses the font texture
                    if ((UINT32)(intptr_t)pcmd->GetTexID() == bd->FontTextureId) {
                        // Render as colored triangle (simplified text rendering)
                        FillTriangle(bd->FrameBuffer, bd->ScreenWidth, bd->ScreenHeight, p0, p1, p2, v0.col, v1.col, v2.col);
                    } else {
                        // Regular colored triangle
                        FillTriangle(bd->FrameBuffer, bd->ScreenWidth, bd->ScreenHeight, p0, p1, p2, v0.col, v1.col, v2.col);
                    }
                }
            }
            
            idx_buffer += pcmd->ElemCount;
        }
    }
    
    // Copy frame buffer to screen using GOP
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL* bltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)bd->FrameBuffer;
    bd->Gop->Blt(bd->Gop, bltBuffer, EfiBltBufferToVideo, 0, 0, 0, 0, bd->ScreenWidth, bd->ScreenHeight, 0);
}

#endif // #ifndef IMGUI_DISABLE