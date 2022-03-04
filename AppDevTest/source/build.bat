echo off
call vcvars64.bat
pushd ..\build
cl ..\source\windows_platform.cpp user32.lib Gdi32.lib
windows_platform.exe
popd
