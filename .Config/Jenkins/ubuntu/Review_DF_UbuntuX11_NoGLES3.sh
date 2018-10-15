#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh
export FSL_FEATURES=[EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV3,OpenGLES2,OpenVX,OpenVX1.1,Vulkan,GoogleUnitTest]

FslBuild.py -t sdk -vv --BuildTime --UseFeatures $FSL_FEATURES -- install
FslBuild.py -t sdk --UseFeatures $FSL_FEATURES --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""$FSL_TEST_REPORTS/(PACKAGE_NAME).xml"""