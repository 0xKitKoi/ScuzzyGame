@echo off
setlocal enabledelayedexpansion

REM === Configuration ===
set PREMAKE_VERSION=5.0.0-beta2
set PREMAKE_URL=https://github.com/premake/premake-core/releases/download/v%PREMAKE_VERSION%/premake-%PREMAKE_VERSION%-windows.zip
set PREMAKE_DIR=tools\premake5

REM === Create directories ===
echo Creating directory: %PREMAKE_DIR%
mkdir %PREMAKE_DIR%

REM === Download Premake ===
echo Downloading Premake %PREMAKE_VERSION%...
curl -L -o premake.zip %PREMAKE_URL%

REM === Extract ===
echo Extracting Premake...
powershell -Command "Expand-Archive -Path 'premake.zip' -DestinationPath '%PREMAKE_DIR%'"

REM === Clean up ZIP ===
del premake.zip

REM === Add Premake to PATH for this session ===
set PATH=%CD%\%PREMAKE_DIR%;%PATH%

REM === Run Premake ===
echo Running Premake...
premake5 vs2022

echo.
echo Setup Complete. Premake Should have generated the visual studio solution with SDL already linked.
pause

