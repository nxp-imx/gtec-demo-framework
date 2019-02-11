#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh

FslBuild.py -t sdk -vv --BuildTime --UseFeatures [EGL,OpenGLES2] --ForAllExe "FslContentSync.py (CONTENT_PATH) $FSL_GRAPHICS_SDK/bin/(PACKAGE_NAME) --ifExist --create -v -i --version"
