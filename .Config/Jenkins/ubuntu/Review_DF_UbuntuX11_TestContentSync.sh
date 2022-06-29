#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

FslBuild.py --noGitHash -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2,Lib_NlohmannJson] --ForAllExe "FslContentSync.py (CONTENT_PATH) $FSL_GRAPHICS_SDK/bin/(PACKAGE_NAME) --ifExist --create -v -i --version"
