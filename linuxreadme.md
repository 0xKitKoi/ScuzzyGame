You'll need cmake.
Sudo apt install cmake build-essential

For Debian-based systems (e.g., Ubuntu):
sudo apt update
sudo apt install -y \
  libsdl2-dev \
  libsdl2-image-dev \
  libsdl2-ttf-dev

For Fedora-based systems:
sudo dnf install -y \
  SDL2-devel \
  SDL2_image-devel \
  SDL2_ttf-devel

For Arch-based systems:
sudo pacman -S --noconfirm \
  sdl2 \
  sdl2_image \
  sdl2_ttf



premake5 gmake
make config=debug_x64
