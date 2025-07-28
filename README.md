
# Another Top-Down RPG | A Scuzzy Game

Another Earthbound/UnderTale inspired game.
No game engine is used, SDL2 is the only dependancy. 
Currently in early alpha stages. I work on this on and off between work and college.
Developmet is streamed live on [twitch.tv](https://twitch.tv/0xshawncena121) 


## Proof of Concept
![In Game Screenshot](https://github.com/0xKitKoi/ScuzzyGame/blob/master/Scuzzy/data/POC.gif)

# Building
This project uses Premake5 for the build system, and depends on SDL2.  
## WINDOWS BUILD
If you are on Windows, Run the SetupSDLWindows.bat. This batch script will download SDL2 and set up the structure for you. 
You also need to run the visualstudiosetup.bat, which will download Premake5 and run it to generate a Visual Studio Solution project.   


Linux users can simply install SDL2 dev libs using their package manager, see the LinuxReadme for help.  
Once SDL2 is setup, simply run premake5 to either make project files like Visual Studio Solution Project or using Make.  
premake5 gmake && make   

## Roadmap

- Undertale/DeltaRune style dodging System ?
- Map Development, [alley, dark alley, dark world entry, boos room]
- Items [bandaid, beef jerky]
- More enemys [2 more low level fights, one boss fight for Beta Release on Steam]
- Story Board needs more coffee and headbanging

