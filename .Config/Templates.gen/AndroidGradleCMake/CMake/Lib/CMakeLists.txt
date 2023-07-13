# stop if cmake version below ##CMAKE_MINIMUM_VERSION##
cmake_minimum_required(VERSION ##CMAKE_MINIMUM_VERSION## FATAL_ERROR)

###########
# Project #
###########

project(##PACKAGE_NAME##)

##########
# Target #
##########
# Add all the include and source files to the library
add_library(##PACKAGE_NAME## STATIC ##PACKAGE_SOURCE_FILES####PACKAGE_INCLUDE_FILES##)
add_library(##ALIAS_PACKAGE_NAME## ALIAS ##PACKAGE_NAME##)
  
##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##
##TARGET_INCLUDE_DIRECTORIES##
#######################
# Target Dependencies #
#######################

##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##
##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##

##SNIPPET_DEFAULT_TARGET_COMPILE_OPTIONS##
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##
endif()
