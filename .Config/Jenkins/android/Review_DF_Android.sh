#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/android/PrepareJenkinsEnvironment.sh

#FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenGLES3.1,Vulkan,EarlyAccess]
#FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenGLES3.1,EarlyAccess]
pushd $FSL_GRAPHICS_SDK/DemoApps/GLES2/S06_Texturing
FslBuild.py -vv --BuildTime --debug -- assembleDebug --info
popd
