#!/bin/bash
set -o errexit

source $WORKSPACE/.Config/Jenkins/ubuntu/PrepareJenkinsEnvironment.sh
export FSL_FEATURES=[EGL,EarlyAccess,OpenCL,OpenCL1.1,OpenCL1.2,OpenCV,OpenCV4,OpenGLES2,OpenGLES3,OpenGLES3.1,OpenVX,OpenVX1.1,Vulkan,GoogleUnitTest]

#FslBuild.py -g legacy -t sdk -vv --BuildTime --Variants [config=Coverage] --UseFeatures $FSL_FEATURES -c install

FslBuild.py -g legacy -t sdk --BuildTime --Variants [config=Coverage] --UseFeatures $FSL_FEATURES --RequireFeature [GoogleUnitTest]

# setup a baseline
lcov --no-external --capture --initial --directory . --output-file coverage_base.info

FslBuild.py -g legacy -t sdk --BuildTime --Variants [config=Coverage] --UseFeatures $FSL_FEATURES --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""$FSL_TEST_REPORTS/(PACKAGE_NAME).xml"""

echo Generating coverage html

# capture data after checks
lcov --no-external --capture --directory . --output-file coverage.info

# combine with baseline
lcov --add-tracefile coverage_base.info --add-tracefile coverage.info --output-file coverage_total.info

# remove output for external libraries
lcov --remove coverage_total.info "/usr*" "$FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR/*" "*/UnitTest/*" "*/FslUnitTest/*" --output-file coverage_total.info

# generate html
genhtml coverage_total.info --output-directory gcov-report

echo Generating coverage xml
lcov_cobertura.py coverage_total.info -d

# gcovr -r . --xml > coverage.xml
# gcovr reports are not visible in jenkins due to security settings
# mkdir -p gcov-report
# gcovr -r . --html --html-details -o gcov-report/index.html
