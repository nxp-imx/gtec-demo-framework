#!/bin/bash

pushd $1
FslBuild.py -r --BuildTime --Variants [config=Coverage] --UseFeatures $FSL_FEATURES --RequireFeature [GoogleUnitTest]

# setup a baseline
lcov --no-external --capture --initial --directory . --output-file coverage_base.info

pushd UnitTest
FslBuild.py --BuildTime --Variants [config=Coverage] --UseFeatures $FSL_FEATURES --RequireFeature [GoogleUnitTest] --ForAllExe "(EXE) --gtest_output=xml:""$FSL_TEST_REPORTS/(PACKAGE_NAME).xml"""
popd

echo Generating coverage html

# capture data after checks
lcov --no-external --capture --directory . --output-file coverage.info

# combine with baseline
lcov --add-tracefile coverage_base.info --add-tracefile coverage.info --output-file coverage_total.info

# remove output for external libraries
lcov --remove coverage_total.info "/usr*" "$FSL_GRAPHICS_SDK_THIRD_PARTY_LIBS_DIR/*" "*/UnitTest/*" "*/FslUnitTest/*" --output-file coverage_total.info

# generate html
genhtml coverage_total.info --output-directory $WORKSPACE/gcov-report

echo Generating coverage xml
lcov_cobertura.py coverage_total.info -d -o $WORKSPACE/coverage.xml
popd
