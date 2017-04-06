@echo off
if "%1" == "x64" goto WIN64
if "%1" == "x86" goto WIN32
if %PROCESSOR_ARCHITECTURE% == AMD64 goto WIN64
if %PROCESSOR_ARCHITEW6432% == AMD64 goto WIN64
:WIN32
echo Win(x86)
echo We only support x64, please upgrade.
exit /b 1
goto START

:WIN64
echo Win(x64)
set FSL_GLES_EMULATOR_PATH=c:\Program Files (x86)\Vivante\vEmulator\x64
goto start

:START
set PATH=%FSL_GLES_EMULATOR_PATH%\bin;%PATH%
set FSL_GLES_INCLUDE_PATH=%FSL_GLES_EMULATOR_PATH%\inc
set FSL_GLES_LIB_PATH=%FSL_GLES_EMULATOR_PATH%\lib
set FSL_GLES_LIB_EGL=libEGL.lib
set FSL_GLES_LIB_GLES=libGLESv2.lib
set FSL_GLES_NAME=VIVANTE