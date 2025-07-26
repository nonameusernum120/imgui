#!/bin/bash

# Demo script for Dear ImGui UEFI Example Artifacts
# This script demonstrates how to download and use the GitHub Action artifacts

set -e

REPO="ocornut/imgui"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Dear ImGui UEFI Example - Artifact Demo"
echo "======================================"
echo ""

show_help() {
    echo "This script demonstrates how to work with GitHub Action artifacts."
    echo ""
    echo "Available commands:"
    echo "  help        - Show this help message"
    echo "  list        - List available workflow runs"
    echo "  download    - Download artifacts from latest successful run"
    echo "  test        - Test downloaded artifacts with QEMU"
    echo "  clean       - Clean up downloaded artifacts"
    echo ""
    echo "Examples:"
    echo "  $0 list"
    echo "  $0 download"
    echo "  $0 test ImGuiUefiExample-X64-bootable.zip"
    echo ""
    echo "Note: You need 'gh' (GitHub CLI) installed for downloading artifacts."
    echo "Install with: sudo apt-get install gh"
}

list_runs() {
    echo "Listing recent UEFI build workflow runs..."
    echo ""
    
    if ! command -v gh &> /dev/null; then
        echo "❌ GitHub CLI (gh) not found!"
        echo "Install with: sudo apt-get install gh"
        echo "Then authenticate with: gh auth login"
        return 1
    fi
    
    echo "Recent 'Build UEFI EFI Image' workflow runs:"
    gh run list --repo "$REPO" --workflow="build-uefi-efi.yml" --limit 10
}

download_artifacts() {
    echo "Downloading artifacts from latest successful run..."
    echo ""
    
    if ! command -v gh &> /dev/null; then
        echo "❌ GitHub CLI (gh) not found!"
        echo "Install with: sudo apt-get install gh"
        echo "Then authenticate with: gh auth login"
        return 1
    fi
    
    # Find the latest successful run
    echo "Finding latest successful run..."
    RUN_ID=$(gh run list --repo "$REPO" --workflow="build-uefi-efi.yml" --status=completed --limit 1 --json databaseId --jq '.[0].databaseId')
    
    if [ -z "$RUN_ID" ]; then
        echo "❌ No successful runs found!"
        echo "Check workflow status at: https://github.com/$REPO/actions/workflows/build-uefi-efi.yml"
        return 1
    fi
    
    echo "Latest run ID: $RUN_ID"
    echo ""
    
    # Create download directory
    DOWNLOAD_DIR="artifacts-$(date +%Y%m%d-%H%M%S)"
    mkdir -p "$DOWNLOAD_DIR"
    cd "$DOWNLOAD_DIR"
    
    echo "Downloading artifacts to: $DOWNLOAD_DIR"
    
    # Download all artifacts
    gh run download "$RUN_ID" --repo "$REPO"
    
    echo ""
    echo "✅ Download complete!"
    echo ""
    echo "Downloaded artifacts:"
    find . -name "*.zip" -o -name "*.efi" -o -name "*.tar.gz" | sort
    
    cd ..
    echo ""
    echo "To test the artifacts:"
    echo "  $0 test $DOWNLOAD_DIR"
}

test_artifacts() {
    local ARTIFACT_DIR="$1"
    
    if [ -z "$ARTIFACT_DIR" ]; then
        echo "Usage: $0 test <artifact-directory-or-file>"
        echo ""
        echo "Examples:"
        echo "  $0 test artifacts-20240101-120000"
        echo "  $0 test ImGuiUefiExample-X64-bootable.zip"
        return 1
    fi
    
    echo "Testing UEFI artifacts..."
    echo ""
    
    # Check if test script exists
    if [ ! -f "$SCRIPT_DIR/test-with-qemu.sh" ]; then
        echo "❌ test-with-qemu.sh not found!"
        echo "Make sure you're running this from the examples/example_uefi directory."
        return 1
    fi
    
    if [ -f "$ARTIFACT_DIR" ] && [[ "$ARTIFACT_DIR" == *.zip ]]; then
        # Single ZIP file
        echo "Testing bootable ZIP: $ARTIFACT_DIR"
        
        # Extract ZIP to temporary directory
        TEMP_DIR=$(mktemp -d)
        unzip -q "$ARTIFACT_DIR" -d "$TEMP_DIR"
        
        # Find the EFI file
        EFI_FILE=$(find "$TEMP_DIR" -name "*.efi" | head -1)
        
        if [ -z "$EFI_FILE" ]; then
            echo "❌ No EFI file found in ZIP!"
            rm -rf "$TEMP_DIR"
            return 1
        fi
        
        echo "Found EFI file: $EFI_FILE"
        echo ""
        
        # Determine architecture from filename
        if [[ "$EFI_FILE" == *"bootx64.efi" ]]; then
            ARCH="x86_64"
        elif [[ "$EFI_FILE" == *"bootia32.efi" ]]; then
            ARCH="i386"
        elif [[ "$EFI_FILE" == *"bootaa64.efi" ]]; then
            ARCH="aarch64"
        else
            ARCH="x86_64"  # default
        fi
        
        echo "Testing with QEMU (architecture: $ARCH)..."
        "$SCRIPT_DIR/test-with-qemu.sh" "$EFI_FILE" "$ARCH"
        
        # Cleanup
        rm -rf "$TEMP_DIR"
        
    elif [ -d "$ARTIFACT_DIR" ]; then
        # Directory with artifacts
        echo "Testing artifacts in directory: $ARTIFACT_DIR"
        
        # Look for bootable ZIP files
        BOOTABLE_ZIPS=$(find "$ARTIFACT_DIR" -name "*bootable*.zip")
        
        if [ -z "$BOOTABLE_ZIPS" ]; then
            echo "❌ No bootable ZIP files found in $ARTIFACT_DIR"
            echo ""
            echo "Available files:"
            find "$ARTIFACT_DIR" -type f | sort
            return 1
        fi
        
        echo "Found bootable packages:"
        echo "$BOOTABLE_ZIPS"
        echo ""
        
        # Test the first X64 bootable package
        X64_ZIP=$(echo "$BOOTABLE_ZIPS" | grep -i x64 | head -1)
        if [ -n "$X64_ZIP" ]; then
            echo "Testing X64 package: $(basename "$X64_ZIP")"
            test_artifacts "$X64_ZIP"
        else
            # Test the first available package
            FIRST_ZIP=$(echo "$BOOTABLE_ZIPS" | head -1)
            echo "Testing package: $(basename "$FIRST_ZIP")"
            test_artifacts "$FIRST_ZIP"
        fi
        
    else
        echo "❌ Invalid artifact path: $ARTIFACT_DIR"
        echo "Must be a directory or a .zip file"
        return 1
    fi
}

clean_artifacts() {
    echo "Cleaning up artifact directories..."
    echo ""
    
    ARTIFACT_DIRS=$(find . -maxdepth 1 -name "artifacts-*" -type d)
    
    if [ -z "$ARTIFACT_DIRS" ]; then
        echo "No artifact directories found to clean."
        return 0
    fi
    
    echo "Found artifact directories:"
    echo "$ARTIFACT_DIRS"
    echo ""
    
    read -p "Delete these directories? (y/N): " -r
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "$ARTIFACT_DIRS" | xargs rm -rf
        echo "✅ Cleanup complete!"
    else
        echo "Cleanup cancelled."
    fi
}

# Main command handling
case "${1:-help}" in
    help|--help|-h)
        show_help
        ;;
    list)
        list_runs
        ;;
    download)
        download_artifacts
        ;;
    test)
        test_artifacts "$2"
        ;;
    clean)
        clean_artifacts
        ;;
    *)
        echo "❌ Unknown command: $1"
        echo ""
        show_help
        exit 1
        ;;
esac