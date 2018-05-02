#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenVX,OpenVX1.1,Vulkan] -- install
