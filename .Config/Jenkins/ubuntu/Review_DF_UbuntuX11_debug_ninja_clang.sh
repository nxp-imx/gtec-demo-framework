#!/bin/bash
set -o errexit

if [ ! -n "${FSL_CI_BUILD_PARAM+1}" ]; then
export FSL_CI_BUILD_PARAM="--CMakeInstallPrefix bin_clang"
fi 

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh
if [ ! -n "${FSL_CI_FEATURES+1}" ]; then
export FSL_CI_FEATURES=[EarlyAccess,EGL,GoogleBenchmark,GoogleUnitTest,OpenCL1.2,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenGLES3.2,OpenVX,OpenVX1.2,Vulkan,Lib_NlohmannJson]
fi 

export FSL_CMAKE_GENERATOR=Ninja
#export FSL_CONFIG_ARGS=-DCMAKE_TOOLCHAIN_FILE=$WORKSPACE/.Config/Jenkins/ubuntu/clang-toolchain.cmake

FslBuild.py --noGitHash -t sdk -vv --BuildTime --Variants [config=Debug] --UseFeatures $FSL_CI_FEATURES --CMakeGeneratorName=$FSL_CMAKE_GENERATOR --CMakeConfigArgs=-DCMAKE_TOOLCHAIN_FILE=$WORKSPACE/.Config/Jenkins/ubuntu/clang-toolchain.cmake
FslBuild.py --noGitHash -t sdk --BuildTime --Variants [config=Debug] --UseFeatures $FSL_CI_FEATURES --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""$FSL_TEST_REPORTS/(PACKAGE_NAME).xml""" --CMakeGeneratorName=$FSL_CMAKE_GENERATOR --CMakeInstallPrefix bin_clang --CMakeConfigArgs=-DCMAKE_TOOLCHAIN_FILE=$WORKSPACE/.Config/Jenkins/ubuntu/clang-toolchain.cmake
