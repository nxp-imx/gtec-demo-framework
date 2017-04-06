@echo off
if "%1" == "x64" goto WIN64
if "%1" == "x86" goto WIN32
if %PROCESSOR_ARCHITECTURE% == AMD64 goto WIN64
if %PROCESSOR_ARCHITEW6432% == AMD64 goto WIN64
:WIN32
echo We only support x64, please upgrade.
exit /b 1

:WIN64
set FSL_GLES_EMULATOR_PATH=c:\Program Files\ARM\Mali Developer Tools\Mali OpenGL ES Emulator 3.0.2.g694a9
goto start

:START
set FSL_GLES_INCLUDE_PATH=%FSL_GLES_EMULATOR_PATH%\include
set FSL_GLES_LIB_PATH=%FSL_GLES_EMULATOR_PATH%
set FSL_GLES_LIB_EGL=libEGL.lib
set FSL_GLES_LIB_GLES=libGLESv2.lib
set FSL_GLES_NAME=ARM
set PATH=%FSL_GLES_LIB_PATH%;%PATH%
