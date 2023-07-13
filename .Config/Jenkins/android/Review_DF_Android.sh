#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/android/PrepareJenkinsEnvironment.sh

#FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenGLES3.1,Vulkan,EarlyAccess,Test_RequireUserInputToExit]
#FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,OpenGLES3,OpenGLES3.1,EarlyAccess,Test_RequireUserInputToExit]

# Due to build time we just build a select few android apps.

pushd $FSL_GRAPHICS_SDK/DemoApps/Vulkan/UI/Benchmark
FslBuild.py --noGitHash -vv --BuildTime --debug -- assembleDebug --info
popd

pushd $FSL_GRAPHICS_SDK/DemoApps/GLES3/UI/Benchmark
FslBuild.py --noGitHash -vv --BuildTime --debug -- assembleDebug --info
popd

pushd $FSL_GRAPHICS_SDK/DemoApps/GLES2/UI/Benchmark
FslBuild.py --noGitHash -vv --BuildTime --debug -- assembleDebug --info
popd
