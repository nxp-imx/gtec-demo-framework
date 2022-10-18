#!/bin/bash
set -o errexit

# Prepare the sdk
echo DF_CI_YOCTO_SDK: ${DF_CI_YOCTO_SDK}
echo DF_CI_YOCTO_SDK_SETUP_SCRIPT: ${DF_CI_YOCTO_SDK_SETUP_SCRIPT}

echo  ****
ls -lan ${DF_CI_YOCTO_SDK}
echo  ****

source $DF_CI_YOCTO_SDK_SETUP_SCRIPT

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

FslBuild.py --noGitHash --debug -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES --Variants [WindowSystem=FB] -c install --CMakeInstallPrefix bin

