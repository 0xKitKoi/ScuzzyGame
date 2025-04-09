@echo off
setlocal enabledelayedexpansion

:: Set up the project directory
set PROJECT_DIR=%cd%

:: Set up the thirdparty folder path
set THIRDPARTY_DIR=%PROJECT_DIR%\thirdparty

:: Create the thirdparty directory if it doesn't exist
if not exist "%THIRDPARTY_DIR%" mkdir "%THIRDPARTY_DIR%"

:: Download and extract precompiled SDL2 if it doesn't exist
if not exist "%THIRDPARTY_DIR%\SDL2" (
    echo Downloading precompiled SDL2...
    powershell -Command "Invoke-WebRequest -Uri https://github.com/libsdl-org/SDL/releases/download/release-2.32.4/SDL2-devel-2.32.4-VC.zip -OutFile %THIRDPARTY_DIR%\SDL2.zip"
    echo Extracting precompiled SDL2...
    powershell -Command "Expand-Archive -Path %THIRDPARTY_DIR%\SDL2.zip -DestinationPath %THIRDPARTY_DIR%"
    move "%THIRDPARTY_DIR%\SDL2-2.32.4" "%THIRDPARTY_DIR%\SDL2"
    del "%THIRDPARTY_DIR%\SDL2.zip"
)

:: Download and extract precompiled SDL_image if it doesn't exist
if not exist "%THIRDPARTY_DIR%\SDL2_image" (
    echo Downloading precompiled SDL_image...
    powershell -Command "Invoke-WebRequest -Uri https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-devel-2.8.8-VC.zip -OutFile %THIRDPARTY_DIR%\SDL_image.zip"
    echo Extracting precompiled SDL_image...
    powershell -Command "Expand-Archive -Path %THIRDPARTY_DIR%\SDL_image.zip -DestinationPath %THIRDPARTY_DIR%"
    move "%THIRDPARTY_DIR%\SDL2_image-2.8.8" "%THIRDPARTY_DIR%\SDL2_image"
    del "%THIRDPARTY_DIR%\SDL_image.zip"
)

:: Download and extract precompiled SDL_ttf if it doesn't exist
if not exist "%THIRDPARTY_DIR%\SDL2_ttf" (
    echo Downloading precompiled SDL_ttf...
    powershell -Command "Invoke-WebRequest -Uri https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-devel-2.24.0-VC.zip -OutFile %THIRDPARTY_DIR%\SDL_ttf.zip"
    echo Extracting precompiled SDL_ttf...
    powershell -Command "Expand-Archive -Path %THIRDPARTY_DIR%\SDL_ttf.zip -DestinationPath %THIRDPARTY_DIR%"
    move "%THIRDPARTY_DIR%\SDL2_ttf-2.24.0" "%THIRDPARTY_DIR%\SDL2_ttf"
    del "%THIRDPARTY_DIR%\SDL_ttf.zip"
)

echo All precompiled libraries have been downloaded and extracted successfully.
pause
