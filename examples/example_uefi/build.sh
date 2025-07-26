#!/bin/bash

# Dear ImGui UEFI Example Build Script
# This script helps build the UEFI application using EDK2

set -e

echo "Dear ImGui UEFI Example Build Script"
echo "====================================="

# Check if we're in an EDK2 environment
if [ -z "$WORKSPACE" ]; then
    echo "Error: WORKSPACE environment variable not set."
    echo "Please run 'source edksetup.sh' from your EDK2 directory first."
    exit 1
fi

# Check if this directory is in the right location relative to EDK2
CURRENT_DIR=$(pwd)
EXPECTED_PATH="$WORKSPACE/ImGui/examples/example_uefi"

if [ "$CURRENT_DIR" != "$EXPECTED_PATH" ]; then
    echo "Warning: Current directory doesn't match expected EDK2 workspace layout"
    echo "Current: $CURRENT_DIR"
    echo "Expected: $EXPECTED_PATH"
    echo ""
    echo "Please ensure Dear ImGui is copied to \$WORKSPACE/ImGui/"
fi

# Determine architecture
ARCH=${1:-X64}
TARGET=${2:-DEBUG}
TOOLCHAIN=${3:-GCC5}

echo "Building with:"
echo "  Architecture: $ARCH"
echo "  Target: $TARGET"
echo "  Toolchain: $TOOLCHAIN"
echo ""

# Build command
BUILD_CMD="build -p ImGui/examples/example_uefi/ImGuiUefiExample.dsc -a $ARCH -t $TOOLCHAIN -b $TARGET"

echo "Executing: $BUILD_CMD"
echo ""

cd "$WORKSPACE"

if $BUILD_CMD; then
    echo ""
    echo "Build successful!"
    echo ""
    echo "Output location:"
    echo "  $WORKSPACE/Build/ImGuiUefiExample/${TARGET}_${TOOLCHAIN}/$ARCH/ImGuiUefiExample.efi"
    echo ""
    echo "To test with QEMU:"
    echo "  1. Create a disk image and copy the EFI file to EFI/BOOT/bootx64.efi"
    echo "  2. Run: qemu-system-x86_64 -bios OVMF.fd -drive format=raw,file=test.img"
    echo ""
else
    echo ""
    echo "Build failed!"
    exit 1
fi