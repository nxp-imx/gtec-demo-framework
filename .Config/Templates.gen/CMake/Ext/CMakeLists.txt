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

##########
# Target #
##########

add_library(##PACKAGE_NAME## INTERFACE)

#Add an alias so that library can be used inside the build tree
add_library(##ALIAS_PACKAGE_NAME## ALIAS ##PACKAGE_NAME##)

# We list these to ensure they appear in the IDE's
# Attach headers
#target_sources(##PACKAGE_NAME## INTERFACE $<BUILD_INTERFACE:#PACKAGE_PUBLIC_INCLUDE_FILES#)

##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##
##TARGET_INCLUDE_DIRECTORIES##

#######################
# Target Dependencies #
#######################

##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##
##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##
##PACKAGE_COPY_FILES##
##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##
##PACKAGE_VARIANT_SETTINGS##
if(INSTALL_LIB)
##PACKAGE_GENERATE_INSTALL_INSTRUCTIONS##
endif()
