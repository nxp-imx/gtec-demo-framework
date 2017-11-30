#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/PrepareJenkinsEnvironment.sh

FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenVX,OpenVX1.0.1,OpenVX1.1,Vulkan] --ForAllExe "FslContentSync.py (CONTENT_PATH) $FSL_GRAPHICS_SDK/bin/(PACKAGE_NAME) --ifExist --create -v -i --version" --BuildThreads 4
