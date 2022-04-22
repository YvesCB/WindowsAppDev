@echo off

set COMMON_FLAGS=-MT -nologo -GR- -EHa- -Od -Oi -WX -W4 -wd4505 -wd4201 -wd4100 -wd4189 -Z7 -Fmwin32_platform.map -DENGINE_INTERNAL=1 -DENGINE_SLOW=1
set LINKER_FLAGS=user32.lib gdi32.lib winmm.lib

call vcvars64.bat
if not exist "..\build\" mkdir "..\build"
pushd ..\build
cl ..\source\win32_platform.cpp %LINKER_FLAGS% %COMMON_FLAGS%
popd
