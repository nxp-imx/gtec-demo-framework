#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

pushd DemoApps
mkdir -p SanityCheck
pushd SanityCheck
FslBuildNew.py --SanityCheck debug -vv --BuildTime --UseFeatures [EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.1,Vulkan] * * --Variants [config=Debug]
popd
popd
