#!/bin/bash
# ITGMania Android Build Environment Setup
# Uses distrobox to create an isolated container with Android SDK/NDK
#
# Usage:
#   ./android-build.sh setup    - Create distrobox container with Android tools
#   ./android-build.sh enter    - Enter the container
#   ./android-build.sh build    - Build the APK inside the container
#   ./android-build.sh clean    - Remove the container

set -e

CONTAINER_NAME="itgmania-android-build"
IMAGE="docker.io/library/ubuntu:22.04"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ANDROID_SDK_VERSION="11076708"  # Command-line tools version
ANDROID_NDK_VERSION="25.2.9519653"
ANDROID_BUILD_TOOLS_VERSION="34.0.0"
ANDROID_PLATFORM_VERSION="34"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_distrobox() {
    if ! command -v distrobox &> /dev/null; then
        log_error "distrobox is not installed. Please install it first."
        log_info "On Fedora: sudo dnf install distrobox"
        log_info "On Ubuntu: sudo apt install distrobox"
        log_info "On Arch: sudo pacman -S distrobox"
        exit 1
    fi
}

setup_container() {
    log_info "Creating distrobox container: $CONTAINER_NAME"
    
    # Create the container
    distrobox create \
        --name "$CONTAINER_NAME" \
        --image "$IMAGE" \
        --yes \
        --home "$PROJECT_DIR/.android-home" \
        --additional-packages "curl wget unzip git openjdk-17-jdk-headless cmake ninja-build build-essential"
    
    log_info "Container created. Installing Android SDK/NDK..."
    
    # Install Android SDK/NDK inside the container
    distrobox enter "$CONTAINER_NAME" -- bash -c "
        set -e
        export ANDROID_HOME=\$HOME/Android/Sdk
        export ANDROID_SDK_ROOT=\$ANDROID_HOME
        export ANDROID_NDK_HOME=\$ANDROID_HOME/ndk/$ANDROID_NDK_VERSION
        
        mkdir -p \$ANDROID_HOME/cmdline-tools
        
        # Download Android command-line tools if not present
        if [ ! -d "\$ANDROID_HOME/cmdline-tools/latest" ]; then
            echo 'Downloading Android Command-line Tools...'
            mkdir -p "\$ANDROID_HOME/cmdline-tools"
            wget -q "https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_SDK_VERSION}_latest.zip" -O "\$ANDROID_HOME/cmdline-tools/tools.zip"
            unzip -q "\$ANDROID_HOME/cmdline-tools/tools.zip" -d "\$ANDROID_HOME/cmdline-tools"
            mv "\$ANDROID_HOME/cmdline-tools/cmdline-tools" "\$ANDROID_HOME/cmdline-tools/latest"
            rm "\$ANDROID_HOME/cmdline-tools/tools.zip"
        fi
        
        export PATH=\$ANDROID_HOME/cmdline-tools/latest/bin:\$PATH
        
        # Accept licenses
        yes | sdkmanager --licenses > /dev/null 2>&1 || true
        
        # Install required SDK components
        echo 'Installing Android SDK components...'
        sdkmanager --install \
            \"platform-tools\" \
            \"build-tools;$ANDROID_BUILD_TOOLS_VERSION\" \
            \"platforms;android-$ANDROID_PLATFORM_VERSION\" \
            \"ndk;$ANDROID_NDK_VERSION\" \
            \"cmake;3.22.1\"
        
        # Create environment setup file
        cat > \$HOME/.android-env << 'ENVEOF'
export ANDROID_HOME=\$HOME/Android/Sdk
export ANDROID_SDK_ROOT=\$ANDROID_HOME
export ANDROID_NDK_HOME=\$ANDROID_HOME/ndk/$ANDROID_NDK_VERSION
export PATH=\$ANDROID_HOME/cmdline-tools/latest/bin:\$ANDROID_HOME/platform-tools:\$PATH
ENVEOF
        
        echo 'Android SDK/NDK installation complete!'
    "
    
    log_info "Distrobox container '$CONTAINER_NAME' is ready!"
    log_info "Run './android-build.sh enter' to enter the container"
    log_info "Run './android-build.sh build' to build the APK"
}

enter_container() {
    log_info "Entering container: $CONTAINER_NAME"
    distrobox enter "$CONTAINER_NAME"
}

build_apk() {
    log_info "Building ITGMania APK..."
    
    distrobox enter "$CONTAINER_NAME" -- bash -c "
        source \$HOME/.android-env 2>/dev/null || true
        cd '$PROJECT_DIR/android'
        
        if [ ! -f 'gradlew' ]; then
            echo 'Error: android/gradlew not found. Make sure the Android project is set up.'
            exit 1
        fi
        
        chmod +x gradlew
        ./gradlew assembleRelease
        
        echo ''
        echo 'Build complete! APK location:'
        find app/build/outputs/apk -name '*.apk' 2>/dev/null | head -5
    "
}

clean_container() {
    log_warn "Removing container: $CONTAINER_NAME"
    distrobox stop "$CONTAINER_NAME" 2>/dev/null || true
    distrobox rm "$CONTAINER_NAME" --force 2>/dev/null || true
    
    if [ -d "$PROJECT_DIR/.android-home" ]; then
        log_info "Note: Container home directory preserved at $PROJECT_DIR/.android-home"
        log_info "Run 'rm -rf $PROJECT_DIR/.android-home' to fully clean up"
    fi
    
    log_info "Container removed"
}

show_help() {
    echo "ITGMania Android Build Script"
    echo ""
    echo "Usage: $0 <command>"
    echo ""
    echo "Commands:"
    echo "  setup   - Create distrobox container with Android SDK/NDK"
    echo "  enter   - Enter the container interactively"
    echo "  build   - Build the APK inside the container"
    echo "  clean   - Remove the container"
    echo "  help    - Show this help message"
    echo ""
    echo "Requirements:"
    echo "  - distrobox must be installed on the host"
    echo "  - Container engine (podman or docker) must be available"
}

# Main
check_distrobox

case "${1:-help}" in
    setup)
        setup_container
        ;;
    enter)
        enter_container
        ;;
    build)
        build_apk
        ;;
    clean)
        clean_container
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        log_error "Unknown command: $1"
        show_help
        exit 1
        ;;
esac
