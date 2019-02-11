#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh
export FSL_FEATURES=[EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.1,Vulkan,GoogleUnitTest]

$WORKSPACE/.Config/Jenkins/ubuntu/CheckUnitTestCoverageForPackagesAt.sh $WORKSPACE/DemoFramework/FslBase
