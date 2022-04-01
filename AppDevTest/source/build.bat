@echo off
call vcvars64.bat
if not exist "..\build\" mkdir "..\build"
pushd ..\build
cl ..\source\windows_platform.cpp user32.lib Gdi32.lib /Z7 /DEBUG
popd
