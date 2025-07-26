/** @file
 * Dear ImGui UEFI Example Application
 * 
 * This is a sample UEFI application that demonstrates the use of Dear ImGui
 * with the UEFI Graphics Output Protocol (GOP).
 * 
 * @author  ImGui UEFI Backend
 * @version 1.0
 */

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimplePointer.h>

// Dear ImGui includes
#include "imgui_c_wrapper.h"
#include "../../backends/imgui_impl_uefi.h"

// Global variables
EFI_GRAPHICS_OUTPUT_PROTOCOL    *gGraphicsOutput = NULL;
EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *gTextInput = NULL;
EFI_SIMPLE_POINTER_PROTOCOL     *gPointer = NULL;

// Application state
typedef struct {
    BOOLEAN ShowDemoWindow;
    BOOLEAN ShowAnotherWindow;
    FLOAT   ClearColor[4];
    FLOAT   FloatValue;
    INT32   Counter;
    CHAR16  TextBuffer[256];
} APP_STATE;

APP_STATE gAppState;

VOID InitializeAppState(VOID)
{
    gAppState.ShowDemoWindow = TRUE;
    gAppState.ShowAnotherWindow = FALSE;
    gAppState.ClearColor[0] = 0.45f;
    gAppState.ClearColor[1] = 0.55f;
    gAppState.ClearColor[2] = 0.60f;
    gAppState.ClearColor[3] = 1.00f;
    gAppState.FloatValue = 0.0f;
    gAppState.Counter = 0;
    StrCpyS(gAppState.TextBuffer, sizeof(gAppState.TextBuffer)/sizeof(CHAR16), L"Hello, UEFI!");
}

/**
 * Initialize graphics and input protocols
 * 
 * @retval EFI_SUCCESS    Protocols initialized successfully
 * @retval EFI_NOT_FOUND  Required protocols not found
 */
EFI_STATUS
InitializeProtocols (
  VOID
  )
{
    EFI_STATUS Status;
    EFI_HANDLE *HandleBuffer;
    UINTN      HandleCount;

    // Locate Graphics Output Protocol
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
    );
    
    if (EFI_ERROR(Status)) {
        Print(L"Failed to locate Graphics Output Protocol: %r\n", Status);
        return Status;
    }

    Status = gBS->HandleProtocol(
        HandleBuffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID**)&gGraphicsOutput
    );
    
    FreePool(HandleBuffer);
    
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open Graphics Output Protocol: %r\n", Status);
        return Status;
    }

    // Locate Simple Text Input Protocol
    Status = gBS->LocateProtocol(
        &gEfiSimpleTextInProtocolGuid,
        NULL,
        (VOID**)&gTextInput
    );
    
    if (EFI_ERROR(Status)) {
        Print(L"Failed to locate Simple Text Input Protocol: %r\n", Status);
        gTextInput = NULL; // Continue without text input
    }

    // Locate Simple Pointer Protocol (optional)
    Status = gBS->LocateProtocol(
        &gEfiSimplePointerProtocolGuid,
        NULL,
        (VOID**)&gPointer
    );
    
    if (EFI_ERROR(Status)) {
        Print(L"Simple Pointer Protocol not available\n");
        gPointer = NULL; // Continue without mouse
    }

    Print(L"Graphics Mode: %dx%d\n", 
          gGraphicsOutput->Mode->Info->HorizontalResolution,
          gGraphicsOutput->Mode->Info->VerticalResolution);

    return EFI_SUCCESS;
}

/**
 * Render the main application UI
 */
VOID
RenderUI (
  VOID
  )
{
    // Main menu bar
    if (ImGui_BeginMainMenuBar()) {
        if (ImGui_BeginMenu("File", TRUE)) {
            if (ImGui_MenuItem("Exit", "Alt+F4", FALSE, TRUE)) {
                // Exit application
            }
            ImGui_EndMenu();
        }
        
        if (ImGui_BeginMenu("View", TRUE)) {
            ImGui_MenuItemPtr("Demo Window", NULL, &gAppState.ShowDemoWindow, TRUE);
            ImGui_MenuItemPtr("Another Window", NULL, &gAppState.ShowAnotherWindow, TRUE);
            ImGui_EndMenu();
        }
        
        if (ImGui_BeginMenu("Help", TRUE)) {
            if (ImGui_MenuItem("About", NULL, FALSE, TRUE)) {
                // Show about dialog
            }
            ImGui_EndMenu();
        }
        
        ImGui_EndMainMenuBar();
    }

    // Demo window
    if (gAppState.ShowDemoWindow) {
        ImGui_ShowDemoWindow(&gAppState.ShowDemoWindow);
    }

    // Custom window
    if (ImGui_Begin("UEFI ImGui Demo", NULL, 0)) {
        ImGui_Text("Hello from UEFI!");
        ImGui_Text("This is running in firmware!");
        
        ImGui_Separator();
        
        // Some controls
        ImGui_SliderFloat("Float", &gAppState.FloatValue, 0.0f, 1.0f, "%.3f", 0);
        ImGui_ColorEdit3("Clear Color", gAppState.ClearColor, 0);
        
        if (ImGui_Button("Button", ImVec2_zero)) {
            gAppState.Counter++;
        }
        
        ImGui_SameLine(0.0f, -1.0f);
        ImGui_Text("Counter = %d", gAppState.Counter);
        
        ImGui_Separator();
        
        // System information
        ImGui_Text("Screen Resolution: %dx%d", 
                   gGraphicsOutput->Mode->Info->HorizontalResolution,
                   gGraphicsOutput->Mode->Info->VerticalResolution);
        
        ImGui_Text("Pixel Format: %d", gGraphicsOutput->Mode->Info->PixelFormat);
        
        // Note: FPS display would require exposing IO through C wrapper
        ImGui_Text("Running in UEFI environment");
    }
    ImGui_End();

    // Another window
    if (gAppState.ShowAnotherWindow) {
        if (ImGui_Begin("Another Window", &gAppState.ShowAnotherWindow, 0)) {
            ImGui_Text("Hello from another window!");
            if (ImGui_Button("Close Me", ImVec2_zero)) {
                gAppState.ShowAnotherWindow = FALSE;
            }
        }
        ImGui_End();
    }
}

/**
 * Main application entry point
 * 
 * @param[in] ImageHandle    The image handle of this application
 * @param[in] SystemTable    The EFI system table
 * 
 * @retval EFI_SUCCESS       Application executed successfully
 * @retval EFI_ABORTED       Application terminated due to error
 */
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;
    BOOLEAN Running = TRUE;
    UINT64 LastTime, CurrentTime, DeltaTime;

    Print(L"Dear ImGui UEFI Example Application\n");
    Print(L"Press ESC to exit\n\n");

    // Initialize application state
    InitializeAppState();

    // Initialize protocols
    Status = InitializeProtocols();
    if (EFI_ERROR(Status)) {
        Print(L"Failed to initialize protocols: %r\n", Status);
        return Status;
    }

    // Initialize Dear ImGui
    ImGui_CheckVersion();
    ImGui_CreateContext();
    
    // Note: For C code, we'll access IO through the backend
    // Setup Dear ImGui style
    ImGui_StyleColorsDark(NULL);

    // Initialize UEFI backend
    if (!ImGui_ImplUefi_Init(gGraphicsOutput, gTextInput, gPointer)) {
        Print(L"Failed to initialize ImGui UEFI backend\n");
        ImGui_DestroyContext();
        return EFI_ABORTED;
    }

    Print(L"ImGui initialized successfully\n");
    Print(L"Entering main loop...\n");

    // Get initial time
    LastTime = GetTimeInNanoSecond(GetPerformanceCounter());

    // Main loop
    while (Running) {
        // Calculate delta time
        CurrentTime = GetTimeInNanoSecond(GetPerformanceCounter());
        DeltaTime = CurrentTime - LastTime;
        LastTime = CurrentTime;

        // Delta time will be handled by the backend
        // (In a more complete implementation, we'd expose IO access through the C wrapper)

        // Check for ESC key to exit
        if (gTextInput != NULL) {
            Status = gTextInput->ReadKeyStroke(gTextInput, &Key);
            if (!EFI_ERROR(Status)) {
                if (Key.ScanCode == SCAN_ESC) {
                    Running = FALSE;
                    continue;
                }
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplUefi_NewFrame();
        ImGui_NewFrame();

        // Render UI
        RenderUI();

        // Rendering
        ImGui_Render();
        ImGui_ImplUefi_RenderDrawData(ImGui_GetDrawData());

        // Small delay to prevent excessive CPU usage
        MicroSecondDelay(16000); // ~60 FPS
    }

    // Cleanup
    Print(L"Shutting down...\n");
    ImGui_ImplUefi_Shutdown();
    ImGui_DestroyContext();

    Print(L"Application terminated successfully\n");
    return EFI_SUCCESS;
}