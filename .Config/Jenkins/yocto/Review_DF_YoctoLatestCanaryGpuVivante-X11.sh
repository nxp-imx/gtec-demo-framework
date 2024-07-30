#!/bin/bash
set -o errexit

# Prepare the sdk
echo DF_CI_YOCTO_SDK: ${DF_CI_YOCTO_SDK}
echo DF_CI_YOCTO_SDK_SETUP_SCRIPT: ${DF_CI_YOCTO_SDK_SETUP_SCRIPT}

echo  ****
ls -lan ${DF_CI_YOCTO_SDK}
echo  ****

source $DF_CI_YOCTO_SDK_SETUP_SCRIPT

if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
  # all
  # export FSL_CI_FEATURES=[Test_RequireUserInputToExit,EarlyAccess,EGL,G2D,GoogleUnitTest,HW_CAMERA_MIPI8X,Lib_NlohmannJson,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenGLES3.2,OpenVG,OpenVX1.2,Vulkan1.2]
  # No GLES3.2, Lib_NlohmannJson, Vulkan, Vulkan1.2
  export FSL_CI_FEATURES=[ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,G2D,GoogleUnitTest,HW_CAMERA_MIPI8X,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX1.2]
fi

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironmentCanaryGpuVivante.sh

FslBuild.py --noGitHash --debug -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES --Variants [WindowSystem=X11] -c install --CMakeInstallPrefix bin
