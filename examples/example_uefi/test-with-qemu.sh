#!/bin/bash

# Test Dear ImGui UEFI Example with QEMU
# This script helps users test the EFI application using QEMU and OVMF firmware

set -e

EFI_FILE="$1"
ARCH="${2:-x86_64}"

if [ -z "$EFI_FILE" ]; then
    echo "Usage: $0 <path-to-efi-file> [architecture]"
    echo ""
    echo "Examples:"
    echo "  $0 ImGuiUefiExample.efi"
    echo "  $0 bootable-image/EFI/BOOT/bootx64.efi x86_64"
    echo ""
    echo "Architecture options:"
    echo "  x86_64 (default) - for X64 EFI files"
    echo "  i386            - for IA32 EFI files"
    echo "  aarch64         - for AARCH64 EFI files"
    exit 1
fi

if [ ! -f "$EFI_FILE" ]; then
    echo "Error: EFI file '$EFI_FILE' not found!"
    exit 1
fi

echo "Testing Dear ImGui UEFI Example with QEMU"
echo "========================================="
echo "EFI File: $EFI_FILE"
echo "Architecture: $ARCH"
echo ""

# Create temporary directory for test
TEST_DIR=$(mktemp -d)
echo "Test directory: $TEST_DIR"

# Create disk image
DISK_IMAGE="$TEST_DIR/test-disk.img"
echo "Creating disk image..."
dd if=/dev/zero of="$DISK_IMAGE" bs=1M count=100 status=progress

# Format as FAT32
echo "Formatting disk image as FAT32..."
mkfs.fat -F 32 "$DISK_IMAGE"

# Mount the disk image
MOUNT_POINT="$TEST_DIR/mount"
mkdir -p "$MOUNT_POINT"
echo "Mounting disk image..."
sudo mount "$DISK_IMAGE" "$MOUNT_POINT"

# Create EFI boot structure
echo "Creating EFI boot structure..."
sudo mkdir -p "$MOUNT_POINT/EFI/BOOT"

# Copy EFI file with correct name based on architecture
case "$ARCH" in
    x86_64)
        BOOT_FILE="bootx64.efi"
        QEMU_ARCH="x86_64"
        QEMU_MACHINE="q35"
        ;;
    i386)
        BOOT_FILE="bootia32.efi"
        QEMU_ARCH="i386"
        QEMU_MACHINE="q35"
        ;;
    aarch64)
        BOOT_FILE="bootaa64.efi"
        QEMU_ARCH="aarch64"
        QEMU_MACHINE="virt"
        ;;
    *)
        echo "Error: Unsupported architecture '$ARCH'"
        sudo umount "$MOUNT_POINT"
        rm -rf "$TEST_DIR"
        exit 1
        ;;
esac

sudo cp "$EFI_FILE" "$MOUNT_POINT/EFI/BOOT/$BOOT_FILE"

# Create a simple startup.nsh script
sudo tee "$MOUNT_POINT/startup.nsh" > /dev/null << EOF
@echo -off
echo "Starting Dear ImGui UEFI Example..."
echo "Press ESC to exit the application"
echo ""
\\EFI\\BOOT\\$BOOT_FILE
EOF

echo "Unmounting disk image..."
sudo umount "$MOUNT_POINT"

# Find OVMF firmware
OVMF_CODE=""
OVMF_VARS=""

# Common OVMF locations
OVMF_LOCATIONS=(
    "/usr/share/ovmf"
    "/usr/share/edk2-ovmf"
    "/usr/share/OVMF"
    "/usr/share/qemu/ovmf"
    "/usr/share/qemu"
)

for location in "${OVMF_LOCATIONS[@]}"; do
    if [ "$ARCH" = "x86_64" ]; then
        if [ -f "$location/OVMF_CODE.fd" ] && [ -f "$location/OVMF_VARS.fd" ]; then
            OVMF_CODE="$location/OVMF_CODE.fd"
            OVMF_VARS="$location/OVMF_VARS.fd"
            break
        elif [ -f "$location/x64/OVMF_CODE.fd" ] && [ -f "$location/x64/OVMF_VARS.fd" ]; then
            OVMF_CODE="$location/x64/OVMF_CODE.fd"
            OVMF_VARS="$location/x64/OVMF_VARS.fd"
            break
        fi
    elif [ "$ARCH" = "i386" ]; then
        if [ -f "$location/OVMF32_CODE_4M.fd" ] && [ -f "$location/OVMF32_VARS_4M.fd" ]; then
            OVMF_CODE="$location/OVMF32_CODE_4M.fd"
            OVMF_VARS="$location/OVMF32_VARS_4M.fd"
            break
        elif [ -f "$location/ia32/OVMF_CODE.fd" ] && [ -f "$location/ia32/OVMF_VARS.fd" ]; then
            OVMF_CODE="$location/ia32/OVMF_CODE.fd"
            OVMF_VARS="$location/ia32/OVMF_VARS.fd"
            break
        fi
    elif [ "$ARCH" = "aarch64" ]; then
        if [ -f "$location/AAVMF_CODE.fd" ] && [ -f "$location/AAVMF_VARS.fd" ]; then
            OVMF_CODE="$location/AAVMF_CODE.fd"
            OVMF_VARS="$location/AAVMF_VARS.fd"
            break
        elif [ -f "$location/aarch64/QEMU_EFI-pflash.raw" ]; then
            OVMF_CODE="$location/aarch64/QEMU_EFI-pflash.raw"
            # For AArch64, we might not need separate VARS
            break
        fi
    fi
done

if [ -z "$OVMF_CODE" ]; then
    echo "Error: OVMF firmware not found!"
    echo ""
    echo "Please install OVMF firmware. On Ubuntu/Debian:"
    echo "  sudo apt-get install ovmf"
    echo ""
    echo "Or download from: https://www.kraxel.org/repos/jenkins/edk2/"
    rm -rf "$TEST_DIR"
    exit 1
fi

echo "Found OVMF firmware:"
echo "  Code: $OVMF_CODE"
echo "  Vars: $OVMF_VARS"
echo ""

# Copy VARS to a writable location
WRITABLE_VARS="$TEST_DIR/OVMF_VARS.fd"
if [ -n "$OVMF_VARS" ]; then
    cp "$OVMF_VARS" "$WRITABLE_VARS"
fi

# Prepare QEMU command
QEMU_CMD="qemu-system-$QEMU_ARCH"

# Check if QEMU is available
if ! command -v "$QEMU_CMD" &> /dev/null; then
    echo "Error: $QEMU_CMD not found!"
    echo ""
    echo "Please install QEMU. On Ubuntu/Debian:"
    echo "  sudo apt-get install qemu-system-x86"
    rm -rf "$TEST_DIR"
    exit 1
fi

echo "Starting QEMU..."
echo "Command: $QEMU_CMD"
echo ""
echo "QEMU Controls:"
echo "  Ctrl+Alt+G: Release mouse grab"
echo "  Ctrl+Alt+Q: Quit QEMU"
echo "  Monitor console: Ctrl+Alt+2 (back to guest: Ctrl+Alt+1)"
echo ""

# Build QEMU arguments
QEMU_ARGS=(
    -machine "$QEMU_MACHINE"
    -cpu qemu64
    -m 256M
    -drive "format=raw,file=$DISK_IMAGE"
    -net none
    -display gtk
    -serial stdio
)

# Add firmware arguments
if [ "$ARCH" = "aarch64" ]; then
    QEMU_ARGS+=(-bios "$OVMF_CODE")
else
    if [ -n "$OVMF_VARS" ]; then
        QEMU_ARGS+=(
            -drive "if=pflash,format=raw,readonly=on,file=$OVMF_CODE"
            -drive "if=pflash,format=raw,file=$WRITABLE_VARS"
        )
    else
        QEMU_ARGS+=(-bios "$OVMF_CODE")
    fi
fi

# Run QEMU
"$QEMU_CMD" "${QEMU_ARGS[@]}"

echo ""
echo "QEMU session ended."

# Cleanup
echo "Cleaning up..."
rm -rf "$TEST_DIR"

echo "Test completed!"