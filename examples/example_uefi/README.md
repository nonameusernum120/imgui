# Dear ImGui UEFI Backend and Example

This directory contains a UEFI backend for Dear ImGui and a sample application that demonstrates its usage.

## Overview

The UEFI backend (`imgui_impl_uefi.h/cpp`) provides a platform and renderer implementation for Dear ImGui that works in the UEFI environment using:

- **Graphics Output Protocol (GOP)** for rendering
- **Simple Text Input Protocol** for keyboard input
- **Simple Pointer Protocol** for mouse input (optional)

## Features

### Implemented
- ✅ Basic rendering using GOP Blt operations
- ✅ Keyboard input support
- ✅ Mouse input support (if available)
- ✅ Texture management
- ✅ Font rendering
- ✅ Basic UI elements (buttons, sliders, text, etc.)

### Limitations
- ❌ No clipboard support (not available in UEFI)
- ❌ No advanced rendering features
- ❌ Simplified text rendering
- ❌ No gamepad support
- ❌ Limited mouse cursor functionality

## Building

### Prerequisites

1. **EDK2 Development Environment**
   - Download and set up EDK2 from [TianoCore](https://github.com/tianocore/edk2)
   - Set up the build environment (Visual Studio on Windows, GCC on Linux)

2. **Toolchain**
   - Windows: Visual Studio 2019/2022 with C++ support
   - Linux: GCC with C++ support
   - LLVM/Clang (alternative)

### Build Steps

1. **Set up EDK2 environment:**
   ```bash
   cd /path/to/edk2
   source edksetup.sh  # Linux/macOS
   # or
   edksetup.bat        # Windows
   ```

2. **Copy ImGui to EDK2 workspace:**
   ```bash
   # Copy the entire imgui directory to your EDK2 workspace
   cp -r /path/to/imgui /path/to/edk2/ImGui
   ```

3. **Build the application:**
   ```bash
   # From EDK2 root directory
   build -p ImGui/examples/example_uefi/ImGuiUefiExample.dsc -a X64 -t GCC5 -b DEBUG
   ```

   Or for Visual Studio on Windows:
   ```cmd
   build -p ImGui\examples\example_uefi\ImGuiUefiExample.dsc -a X64 -t VS2019 -b DEBUG
   ```

4. **Locate the built EFI file:**
   - The compiled application will be in `Build/ImGuiUefiExample/DEBUG_*/X64/ImGuiUefiExample.efi`

## Testing

### Using QEMU

1. **Create a test image:**
   ```bash
   # Create a disk image
   qemu-img create -f raw test.img 100M
   
   # Format as FAT32
   mkfs.fat -F 32 test.img
   
   # Mount and copy the EFI file
   mkdir mnt
   sudo mount test.img mnt
   sudo mkdir -p mnt/EFI/BOOT
   sudo cp Build/ImGuiUefiExample/DEBUG_*/X64/ImGuiUefiExample.efi mnt/EFI/BOOT/bootx64.efi
   sudo umount mnt
   ```

2. **Run with QEMU:**
   ```bash
   qemu-system-x86_64 -bios /path/to/OVMF.fd -drive format=raw,file=test.img
   ```

### Using Real Hardware

1. **Prepare USB drive:**
   - Format a USB drive with FAT32
   - Create directory structure: `EFI/BOOT/`
   - Copy the compiled EFI file as `bootx64.efi`

2. **Boot from USB:**
   - Insert USB drive
   - Enter BIOS/UEFI settings
   - Disable Secure Boot (unless you sign the executable)
   - Set USB as first boot device
   - Reboot

## Usage

### Controls

- **ESC**: Exit the application
- **Keyboard**: Standard text input and navigation
- **Mouse**: Click and drag (if Simple Pointer Protocol is available)

### UI Elements

The example application demonstrates:
- Main menu bar
- Demo window (Dear ImGui's built-in demo)
- Custom windows with various controls
- System information display
- Real-time performance metrics

## Integration Guide

To integrate the UEFI backend into your own UEFI application:

1. **Include the backend files:**
   ```c
   #include "imgui.h"
   #include "imgui_impl_uefi.h"
   ```

2. **Initialize Dear ImGui:**
   ```c
   IMGUI_CHECKVERSION();
   ImGui_CreateContext();
   ImGuiIO* io = ImGui_GetIO();
   
   // Configure as needed
   io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
   
   // Initialize backend
   ImGui_ImplUefi_Init(GraphicsOutputProtocol, TextInputProtocol, PointerProtocol);
   ```

3. **Main loop:**
   ```c
   while (running) {
       // Handle timing
       io->DeltaTime = CalculateDeltaTime();
       
       // New frame
       ImGui_ImplUefi_NewFrame();
       ImGui_NewFrame();
       
       // Your UI code here
       ImGui_Begin("My Window", NULL, 0);
       ImGui_Text("Hello, UEFI!");
       ImGui_End();
       
       // Render
       ImGui_Render();
       ImGui_ImplUefi_RenderDrawData(ImGui_GetDrawData());
   }
   ```

4. **Cleanup:**
   ```c
   ImGui_ImplUefi_Shutdown();
   ImGui_DestroyContext();
   ```

## Architecture Details

### Rendering Pipeline

1. **Frame Buffer**: Software rendering to an off-screen buffer
2. **Triangle Rasterization**: Basic triangle filling with barycentric coordinates
3. **GOP Transfer**: Copy frame buffer to screen using Graphics Output Protocol

### Input Handling

- **Keyboard**: Polls Simple Text Input Protocol for key events
- **Mouse**: Polls Simple Pointer Protocol for movement and buttons
- **Event Processing**: Converts UEFI input events to Dear ImGui events

### Memory Management

- Uses UEFI's `AllocatePool`/`FreePool` for dynamic allocation
- Manages texture storage in system memory
- Frame buffer allocated based on screen resolution

## Troubleshooting

### Common Issues

1. **Build Errors:**
   - Ensure C++ support is properly configured
   - Check that all required EDK2 packages are available
   - Verify toolchain compatibility

2. **Missing Library Errors (e.g., RegisterFilterLib):**
   - Use `ImGuiUefiExample_Minimal.dsc` for older EDK2 versions
   - Some libraries like RegisterFilterLib were added in newer EDK2 releases
   - If you get library class not found errors, try the minimal DSC file:
     ```bash
     build -p ImGui/examples/example_uefi/ImGuiUefiExample_Minimal.dsc -a X64 -t GCC5 -b DEBUG
     ```

3. **Runtime Issues:**
   - Verify Graphics Output Protocol is available
   - Check screen resolution compatibility
   - Ensure sufficient memory for frame buffer

4. **Input Problems:**
   - Text input may not work if Simple Text Input Protocol is unavailable
   - Mouse input requires Simple Pointer Protocol support

### Debugging

- Use UEFI's `Print()` function for debug output
- Check return values from protocol initialization
- Verify memory allocation success

## Performance Considerations

- Software rendering is CPU-intensive
- Frame buffer size depends on screen resolution
- Consider reducing UI complexity for better performance
- Target 30-60 FPS depending on hardware capabilities

## License

This UEFI backend follows the same license as Dear ImGui (MIT License).

## Contributing

Contributions are welcome! Areas for improvement:
- Better text rendering with proper font texture sampling
- Optimized triangle rasterization
- Additional input device support
- Hardware-accelerated rendering where available