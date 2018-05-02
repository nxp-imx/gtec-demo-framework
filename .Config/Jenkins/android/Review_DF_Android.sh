#!/bin/bash
set -o errexit
export FSL_GRAPHICS_SDK=$WORKSPACE
export FSL_GRAPHICS_SDK_ANDROID_PROJECT_DIR=$FSL_GRAPHICS_SDK/Android
export FSL_PLATFORM_NAME=Android

. prepare.sh
#FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenGLES3.1,Vulkan,EarlyAccess]
#FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenGLES3.1,EarlyAccess]
pushd $FSL_GRAPHICS_SDK/DemoApps/GLES2/S06_Texturing
FslBuild.py -vv --BuildTime --debug -- assembleDebug --info
popd
