@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" x64
mkdir ..\build
pushd ..\build

cl -Zi ..\code\win32_byhand.cpp User32.lib gdi32.lib

popd

