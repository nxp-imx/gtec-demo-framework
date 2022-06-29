#!/bin/bash
set -o errexit

# Prepare the sdk
source /opt/fsl-imx-internal-xwayland/5.10-hardknott/environment-setup-cortexa35-poky-linux

source $WORKSPACE/.Config/Jenkins/yocto/PrepareJenkinsEnvironment.sh

FslBuild.py --noGitHash --debug -t sdk -vv --BuildTime --UseFeatures $FSL_CI_FEATURES --Variants [WindowSystem=FB] -c install --CMakeInstallPrefix bin



