#!/usr/bin/env bash
set -e

INSTALL_DIR="${1:-$(dirname "$0")/lib}"

echo "Installing SDL2 and dependencies into $INSTALL_DIR..."

mkdir -p "$INSTALL_DIR"

INSTALL_FULL_DIR=$(realpath "$INSTALL_DIR")

REPOS=(
    "https://github.com/libsdl-org/SDL.git"
    "https://github.com/libsdl-org/SDL_image.git"
    "https://github.com/libsdl-org/SDL_mixer.git"
    "https://github.com/libsdl-org/SDL_ttf.git"
)

install_lib() {
  local repo_url=$1
  local recurse=$2

  NAME=$(basename "$REPO" .git)
      SRC_DIR="$INSTALL_DIR/$NAME"
      BUILD_DIR="$SRC_DIR/build"

      echo -e "\n=== Installing $NAME ==="
      if [ ! -d "$SRC_DIR" ]; then
        if [ "$recurse" = true ]; then
          git clone --recurse-submodules "$REPO" "$SRC_DIR"
        else
          git clone "$REPO" "$SRC_DIR"
        fi
      fi

      pushd "$SRC_DIR" > /dev/null
      git checkout SDL2
      popd > /dev/null

      mkdir -p "$BUILD_DIR"
      pushd "$BUILD_DIR" > /dev/null

      if ! cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_FULL_DIR" -DCMAKE_BUILD_TYPE=Release -DSDL3_DIR="$INSTALL_FULL_DIR/cmake" .. && [ "$recurse" = false ]; then
        echo "CMake configuration failed for $NAME, retrying with submodules..."
        rm -rf "$SRC_DIR"
        install_lib "$REPO" true
        return
      fi
      if ! cmake --build . --config Release --target install -- -j$(nproc); then
        echo "Build or installation failed for $NAME"
        exit 1
      fi

      popd > /dev/null

      rm -rf "$SRC_DIR" 2> /dev/null || true

}

for REPO in "${REPOS[@]}"; do
  install_lib "$REPO" false
done

echo -e "\nSDL3 installation complete in $INSTALL_DIR"
exit 0
