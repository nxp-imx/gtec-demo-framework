@echo off
set FSL_GRAPHICS_SDK=%cd%
rem If ANT_HOME is defined make sure we have access to the bin directory
if defined ANT_HOME set PATH=%path%;%ANT_HOME%\bin
if defined OPENCV_DIR set PATH=%path%;%OPENCV_DIR%\x64\vc14\bin

if not defined FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR (
  set FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=%FSL_GRAPHICS_SDK%\.Thirdparty
  echo WARNING: Environment variable FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR was not defined, so we force defined it.
  echo          It's highly recommended to manually define this variable so you can reuse the thirdparty libs between builds and repositories.
  echo          Read Doc\BuildingExternals.md for detailed information about this environment variable.
)

if not defined FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR set FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=%FSL_GRAPHICS_SDK%\Android

rem Setup the paths to the DLL's we depend on. We add them first in the path to ensure the dll lookup finds the right one
set PATH=%FSL_GRAPHICS_SDK%\.Config;%path%
  
echo FSL_GRAPHICS_SDK=%FSL_GRAPHICS_SDK%
echo FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=%FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR%
echo FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR=%FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR%
