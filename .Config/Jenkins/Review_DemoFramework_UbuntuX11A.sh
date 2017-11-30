#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/PrepareJenkinsEnvironment.sh

FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.0.1,OpenVX1.1,Vulkan] --BuildThreads 4
