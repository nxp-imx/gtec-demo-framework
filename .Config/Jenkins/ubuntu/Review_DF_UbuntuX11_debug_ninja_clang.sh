#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh
export FSL_FEATURES=[EarlyAccess,EGL,GoogleUnitTest,OpenCL1.2,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.1,Vulkan]
export FSL_CMAKE_GENERATOR=Ninja
#export FSL_CONFIG_ARGS=-DCMAKE_TOOLCHAIN_FILE=$WORKSPACE/.Config/Jenkins/ubuntu/clang-toolchain.cmake

FslBuild.py -t sdk -vv --BuildTime --Variants [config=Debug] --UseFeatures $FSL_FEATURES --CMakeGeneratorName=$FSL_CMAKE_GENERATOR --CMakeInstallPrefix bin_clang --CMakeConfigArgs=-DCMAKE_TOOLCHAIN_FILE=$WORKSPACE/.Config/Jenkins/ubuntu/clang-toolchain.cmake
FslBuild.py -t sdk --BuildTime --Variants [config=Debug] --UseFeatures $FSL_FEATURES --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""$FSL_TEST_REPORTS/(PACKAGE_NAME).xml""" --CMakeGeneratorName=$FSL_CMAKE_GENERATOR --CMakeInstallPrefix bin_clang --CMakeConfigArgs=-DCMAKE_TOOLCHAIN_FILE=$WORKSPACE/.Config/Jenkins/ubuntu/clang-toolchain.cmake
