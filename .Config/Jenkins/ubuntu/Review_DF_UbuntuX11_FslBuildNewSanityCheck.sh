#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

pushd DemoApps
mkdir -p SanityCheck
pushd SanityCheck
FslBuildNew.py --noGitHash --SanityCheck debug -vv --BuildTime --UseFeatures [ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,OpenCL1.2,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.2,Vulkan,Lib_NlohmannJson,Lib_pugixml] * *
popd
popd
