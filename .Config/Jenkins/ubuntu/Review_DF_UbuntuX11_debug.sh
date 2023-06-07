#!/bin/bash
set -o errexit

if [ ! -n "${FSL_CI_BUILD_PARAM+1}" ]; then
export FSL_CI_BUILD_PARAM="-c install --CMakeInstallPrefix bin"
fi 

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
export FSL_CI_FEATURES=[ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,GoogleBenchmark,GoogleUnitTest,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenGLES3.2,OpenVG,OpenVX1.2,Vulkan,Lib_NlohmannJson]
fi 

FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES --Variants [config=Debug] $FSL_CI_BUILD_PARAM
FslBuild.py --noGitHash -t sdk --BuildTime --UseFeatures $FSL_CI_FEATURES --RequireFeature [GoogleUnitTest] --Variants [config=Debug] --ForAllExe "(EXE) --gtest_output=xml:""$FSL_TEST_REPORTS/(PACKAGE_NAME).xml"""
