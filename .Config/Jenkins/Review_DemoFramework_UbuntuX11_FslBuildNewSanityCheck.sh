#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/PrepareJenkinsEnvironment.sh

pushd DemoApps
mkdir SanityCheck
pushd SanityCheck
FslBuildNew.py --SanityCheck debug -vv --BuildTime --UseFeatures [EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenVX,OpenVX1.0.1,OpenVX1.1,Vulkan] --BuildThreads 4 * *
popd
popd
