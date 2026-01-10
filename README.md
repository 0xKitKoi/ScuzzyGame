
# A Scuzzy Game

This game is inspired by: EarthBound, DeltaRune, and Undertale. <br>
No game engine is used, SDL2 is the only dependancy. 
Currently in early alpha stages. I work on this on and off between work and college.
Development is streamed live on [twitch.tv](https://twitch.tv/0xshawncena121) 


## Proof of Concept
![In Game Screenshot](https://github.com/0xKitKoi/ScuzzyGame/blob/master/Scuzzy/data/POC.gif)


## Roadmap
- Map Development, [alley, dark alley, dark world entry, boss room]
- Items [bandaid, beef jerky, KEY Items for story or something]
- More enemys [2 more low level fights, one boss fight for Beta Release on Steam]
- Story Board needs more coffee and headbanging


# Building
This project uses Premake5 for the build system, and depends on SDL2.  
## WINDOWS BUILD
Run the ```SetupSDLWindows.bat```. This batch script will download SDL2 and set up the structure for you. <br>
You also need to run the ```visualstudiosetup.bat```, which will download Premake5 and run it to generate a Visual Studio Solution project.   


## LINUX BUILD
You'll need cmake and Simple Direct Media Layer 2 (SDL2) :
```bash
# For Debian-based systems (e.g., Ubuntu):
sudo apt update
sudo apt install cmake build-essential
sudo apt install -y \
  libsdl2-dev \
  libsdl2-image-dev \
  libsdl2-ttf-dev

# For Fedora-based systems:
sudo dnf install -y \
  SDL2-devel \
  SDL2_image-devel \
  SDL2_ttf-devel

# For Arch-based systems:
sudo pacman -S --noconfirm \
  sdl2 \
  sdl2_image \
  sdl2_ttf
```

we use premake5 to make the build scripts:
[Premake Releases](https://github.com/premake/premake-core/releases)

```bash
wget https://github.com/premake/premake-core/releases/download/v5.0.0-beta7/premake-5.0.0-beta7-src.zip
```
untar or unzip it and use it like so:
```bash
premake5 gmake
make config=debug_x64
```

you can also make vs code on linux work better using bear:
```bash
sudo apt install bear clangd
make clean
bear -- make
```
this makes a ```compile_commands.json```.
make this file: ```.vscode/c_cpp_properties.json``` and place this in it:
```json
{
  "version": 4,
  "configurations": [
    {
      "name": "Linux",
      "compileCommands": "${workspaceFolder}/compile_commands.json"
    }
  ]
}
```
reload the vscode window, this should help with Include path errors.
#### I build with this command: ```make && cd bin/Debug && ./Scuzzy && cd -```


