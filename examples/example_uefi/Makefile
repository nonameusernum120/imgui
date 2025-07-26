# Makefile for Dear ImGui UEFI Example
# This provides basic compilation testing without requiring full EDK2 setup

# Compiler settings
CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -fno-rtti -fno-exceptions -Wall -Wformat -Wno-unused-variable -Wno-unused-function
CFLAGS = -std=c99 -Wall -Wformat
INCLUDES = -I. -I../.. -Ifake_edk2/MdePkg/Include -Ifake_edk2/MdePkg/Include/X64
DEFINES = -DDISABLE_NEW_DEPRECATED_INTERFACES

# Source files
IMGUI_SOURCES = ../../imgui.cpp ../../imgui_demo.cpp ../../imgui_draw.cpp ../../imgui_tables.cpp ../../imgui_widgets.cpp
BACKEND_SOURCES = ../../backends/imgui_impl_uefi.cpp
WRAPPER_SOURCES = imgui_c_wrapper.cpp

# Object files
IMGUI_OBJS = $(IMGUI_SOURCES:.cpp=.o)
BACKEND_OBJS = $(BACKEND_SOURCES:.cpp=.o)
WRAPPER_OBJS = $(WRAPPER_SOURCES:.cpp=.o)

# Default target
all: setup_fake_headers compile_test

# Create minimal UEFI headers for compilation testing
setup_fake_headers:
	@echo "Setting up fake EDK2 headers for compilation testing..."
	@mkdir -p fake_edk2/MdePkg/Include/X64
	@mkdir -p fake_edk2/MdePkg/Include/Protocol
	@mkdir -p fake_edk2/MdePkg/Include/Library
	@echo '#ifndef __UEFI_H__' > fake_edk2/MdePkg/Include/Uefi.h
	@echo '#define __UEFI_H__' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef unsigned char       BOOLEAN;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef signed char         INT8;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef unsigned char       UINT8;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef signed short        INT16;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef unsigned short      UINT16;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef signed int          INT32;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef unsigned int        UINT32;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef signed long long    INT64;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef unsigned long long  UINT64;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef void                VOID;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef UINT64              UINTN;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef UINT64              EFI_STATUS;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef UINT16              CHAR16;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef UINT8               CHAR8;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef float               FLOAT;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef UINTN               uintptr_t;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo '#define CONST const' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo '#define TRUE  1' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo '#define FALSE 0' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo '#define EFI_ERROR(x) ((x) != 0)' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef struct { UINT32 Data1; } EFI_GUID;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef void* EFI_HANDLE;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef struct { void* dummy; } EFI_SYSTEM_TABLE;' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo '#define EFIAPI' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo 'typedef EFI_STATUS (EFIAPI *EFI_FUNCTION)(void);' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Uefi.h
	@echo '#ifndef __GRAPHICS_OUTPUT_H__' > fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo '#define __GRAPHICS_OUTPUT_H__' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo '#include "../Uefi.h"' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo 'typedef struct { UINT8 Blue, Green, Red, Reserved; } EFI_GRAPHICS_OUTPUT_BLT_PIXEL;' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo 'typedef enum { EfiBltVideoFill, EfiBltVideoToBltBuffer, EfiBltBufferToVideo, EfiBltVideoToVideo } EFI_GRAPHICS_OUTPUT_BLT_OPERATION;' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo 'typedef struct { UINT32 HorizontalResolution, VerticalResolution, PixelFormat; } EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo 'typedef struct { EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* Info; } EFI_GRAPHICS_OUTPUT_MODE;' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo 'typedef struct { EFI_GRAPHICS_OUTPUT_MODE* Mode; EFI_STATUS (*Blt)(void*,void*,UINT32,UINTN,UINTN,UINTN,UINTN,UINTN,UINTN,UINTN); } EFI_GRAPHICS_OUTPUT_PROTOCOL;' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Protocol/GraphicsOutput.h
	@echo '#ifndef __SIMPLE_TEXT_IN_H__' > fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define __SIMPLE_TEXT_IN_H__' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#include "../Uefi.h"' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo 'typedef struct { UINT16 ScanCode; CHAR16 UnicodeChar; } EFI_INPUT_KEY;' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo 'typedef struct { EFI_STATUS (*ReadKeyStroke)(void*, EFI_INPUT_KEY*); } EFI_SIMPLE_TEXT_INPUT_PROTOCOL;' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_NULL 0x00' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_UP 0x01' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_DOWN 0x02' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_RIGHT 0x03' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_LEFT 0x04' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_HOME 0x05' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_END 0x06' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_INSERT 0x07' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_DELETE 0x08' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_PAGE_UP 0x09' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_PAGE_DOWN 0x0A' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F1 0x0B' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F2 0x0C' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F3 0x0D' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F4 0x0E' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F5 0x0F' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F6 0x10' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F7 0x11' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F8 0x12' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F9 0x13' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F10 0x14' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F11 0x15' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_F12 0x16' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#define SCAN_ESC 0x17' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Protocol/SimpleTextIn.h
	@echo '#ifndef __SIMPLE_POINTER_H__' > fake_edk2/MdePkg/Include/Protocol/SimplePointer.h
	@echo '#define __SIMPLE_POINTER_H__' >> fake_edk2/MdePkg/Include/Protocol/SimplePointer.h
	@echo '#include "../Uefi.h"' >> fake_edk2/MdePkg/Include/Protocol/SimplePointer.h
	@echo 'typedef struct { INT32 RelativeMovementX, RelativeMovementY; BOOLEAN LeftButton, RightButton; } EFI_SIMPLE_POINTER_STATE;' >> fake_edk2/MdePkg/Include/Protocol/SimplePointer.h
	@echo 'typedef struct { EFI_STATUS (*GetState)(void*, EFI_SIMPLE_POINTER_STATE*); } EFI_SIMPLE_POINTER_PROTOCOL;' >> fake_edk2/MdePkg/Include/Protocol/SimplePointer.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Protocol/SimplePointer.h
	@echo '#ifndef __UEFI_LIB_H__' > fake_edk2/MdePkg/Include/Library/UefiLib.h
	@echo '#define __UEFI_LIB_H__' >> fake_edk2/MdePkg/Include/Library/UefiLib.h
	@echo '#include "../Uefi.h"' >> fake_edk2/MdePkg/Include/Library/UefiLib.h
	@echo 'UINTN Print(CONST CHAR16* Format, ...);' >> fake_edk2/MdePkg/Include/Library/UefiLib.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Library/UefiLib.h
	@echo '#ifndef __MEMORY_ALLOCATION_LIB_H__' > fake_edk2/MdePkg/Include/Library/MemoryAllocationLib.h
	@echo '#define __MEMORY_ALLOCATION_LIB_H__' >> fake_edk2/MdePkg/Include/Library/MemoryAllocationLib.h
	@echo '#include "../Uefi.h"' >> fake_edk2/MdePkg/Include/Library/MemoryAllocationLib.h
	@echo 'VOID* AllocatePool(UINTN Size);' >> fake_edk2/MdePkg/Include/Library/MemoryAllocationLib.h
	@echo 'VOID* AllocateZeroPool(UINTN Size);' >> fake_edk2/MdePkg/Include/Library/MemoryAllocationLib.h
	@echo 'VOID FreePool(VOID* Buffer);' >> fake_edk2/MdePkg/Include/Library/MemoryAllocationLib.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Library/MemoryAllocationLib.h
	@echo '#ifndef __BASE_MEMORY_LIB_H__' > fake_edk2/MdePkg/Include/Library/BaseMemoryLib.h
	@echo '#define __BASE_MEMORY_LIB_H__' >> fake_edk2/MdePkg/Include/Library/BaseMemoryLib.h
	@echo '#include "../Uefi.h"' >> fake_edk2/MdePkg/Include/Library/BaseMemoryLib.h
	@echo 'VOID SetMem(VOID* Buffer, UINTN Length, UINT8 Value);' >> fake_edk2/MdePkg/Include/Library/BaseMemoryLib.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Library/BaseMemoryLib.h
	@echo '#ifndef __BASE_LIB_H__' > fake_edk2/MdePkg/Include/Library/BaseLib.h
	@echo '#define __BASE_LIB_H__' >> fake_edk2/MdePkg/Include/Library/BaseLib.h
	@echo '#include "../Uefi.h"' >> fake_edk2/MdePkg/Include/Library/BaseLib.h
	@echo 'UINTN StrLen(CONST CHAR16* String);' >> fake_edk2/MdePkg/Include/Library/BaseLib.h
	@echo 'CHAR16* StrCpyS(CHAR16* Dest, UINTN DestMax, CONST CHAR16* Src);' >> fake_edk2/MdePkg/Include/Library/BaseLib.h
	@echo '#endif' >> fake_edk2/MdePkg/Include/Library/BaseLib.h

# Compilation test target
compile_test:
	@echo "Testing Dear ImGui core compilation for UEFI..."
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $(DEFINES) ../../imgui.cpp -o imgui.o
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $(DEFINES) ../../imgui_draw.cpp -o imgui_draw.o
	@echo "✅ Dear ImGui core compiled successfully for UEFI"
	
	@echo "Testing UEFI backend compilation..."
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $(DEFINES) ../../backends/imgui_impl_uefi.cpp -o imgui_impl_uefi.o
	@echo "✅ UEFI backend compiled successfully"
	
	@echo ""
	@echo "🎉 All UEFI compilation tests passed!"
	@echo ""
	@echo "Note: This is a compilation test only."
	@echo "For full UEFI build, use EDK2 with:"
	@echo "  build -p ImGui/examples/example_uefi/ImGuiUefiExample.dsc -a X64 -t GCC5 -b DEBUG"

# Clean up
clean:
	rm -f *.o
	rm -f ../../*.o
	rm -f ../../backends/*.o
	rm -rf fake_edk2

# Help target
help:
	@echo "Dear ImGui UEFI Example Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Set up fake headers and run compilation tests"
	@echo "  compile_test - Test compilation of UEFI backend and Dear ImGui core"
	@echo "  clean        - Remove object files and fake headers"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Note: This Makefile is for compilation testing only."
	@echo "For actual UEFI builds, use EDK2 build system."

.PHONY: all setup_fake_headers compile_test clean help