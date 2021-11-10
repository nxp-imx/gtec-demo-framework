#!/bin/bash
set -o errexit

if [ ! -n "${FSL_CI_BUILD_PARAM+1}" ]; then
export FSL_CI_BUILD_PARAM="--CMakeInstallPrefix bin"
fi 

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
export FSL_CI_FEATURES=[EarlyAccess,EGL,GoogleUnitTest,OpenCL1.2,OpenCV4,OpenGLES2,OpenVX,OpenVX1.1,Vulkan,Lib_NlohmannJson]
fi

FslBuild.py -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES $FSL_CI_BUILD_PARAM
FslBuild.py -t sdk --BuildTime --UseFeatures $FSL_CI_FEATURES --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""$FSL_TEST_REPORTS/(PACKAGE_NAME).xml"""