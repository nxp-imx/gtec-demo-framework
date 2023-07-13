#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh
export FSL_FEATURES=[ConsoleHost,WindowHost,Test_RequireUserInputToExit,EarlyAccess,EGL,OpenCL1.2,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.2,Vulkan,GoogleUnitTest,Lib_NlohmannJson,Lib_pugixml]

$WORKSPACE/.Config/Jenkins/ubuntu/CheckUnitTestCoverageForPackagesAt.sh $WORKSPACE/DemoFramework/FslBase
