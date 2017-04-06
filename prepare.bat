@echo off
set FSL_GRAPHICS_SDK=%cd%
set PATH=%FSL_GRAPHICS_SDK%\.Config;%FSL_GRAPHICS_SDK%\ThirdParty\Precompiled\DevIL-SDK-x64-1.8.0\lib\x64\unicode\Release;%FSL_GRAPHICS_SDK%\ThirdParty\Precompiled\assimp-3.3.1\bin;%FSL_GRAPHICS_SDK%\ThirdParty\Precompiled\zlib-1.2.8\bin;%FSL_GRAPHICS_SDK%/ThirdParty/Precompiled/amdovx-core-0.9-beta1/lib;%AMDAPPSDKROOT%\bin\x86_64;%path%
rem If ANT_HOME is defined make sure we have access to the bin directory
if defined ANT_HOME set PATH=%path%;%ANT_HOME%\bin
if defined OPENCV_DIR set PATH=%path%;%OPENCV_DIR%\x64\vc14\bin
if defined OPENCV2_4_DIR set PATH=%path%;%OPENCV2_4_DIR%\x64\vc12\bin

if not defined FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR set FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=%FSL_GRAPHICS_SDK%\Android
  
echo FSL_GRAPHICS_SDK=%FSL_GRAPHICS_SDK%
echo FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=%FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR%
