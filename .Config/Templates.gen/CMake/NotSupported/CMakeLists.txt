##COMMON_HEADER##

##########
# CHECKS #
##########

##DEFINE_PATH_ENVIRONMENT_AS_VARIABLES##

###########
# Modules #
###########

##SNIPPET_COMMON_MODULES##

###########
# Options #
###########

option(CODE_COVERAGE "Enable code coverage (only works with GCC)" OFF)
##SNIPPET_OPTIONS_LIB##

#########
# Cache #
#########

##SNIPPET_CACHE_VARIANTS##

###########
# Project #
###########

project(
  ##PACKAGE_NAME##
  VERSION ##PROJECT_VERSION##
  LANGUAGES CXX
)

##PROJECT_PREVENT_IN_SOURCE_BUILDS##


MESSAGE(STATUS "##PACKAGE_NAME## is marked as not supported on this platform")