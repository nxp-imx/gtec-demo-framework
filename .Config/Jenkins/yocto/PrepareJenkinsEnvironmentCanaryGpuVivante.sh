#!/bin/bash

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironmentBase.sh

if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
  # all
  # export FSL_CI_FEATURES=[ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,G2D,GoogleUnitTest,HW_CAMERA_MIPI8X,HW_GPU_VIVANTE,Lib_NlohmannJson,Lib_pugixml,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenGLES3.2,OpenVG,OpenVX1.2,Vulkan1.2]
  # No GLES3.2
  # export FSL_CI_FEATURES=[ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,G2D,GoogleUnitTest,HW_CAMERA_MIPI8X,Lib_NlohmannJson,Lib_pugixml,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVG,OpenVX1.2,Vulkan1.2]
  # No GLES3.2
  export FSL_CI_FEATURES=[ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,G2D,GoogleUnitTest,HW_CAMERA_MIPI8X,HW_GPU_VIVANTE,Lib_NlohmannJson,Lib_pugixml,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3.1,OpenVG,OpenVX1.2,Vulkan1.2]
fi
