# stop if cmake version below ##CMAKE_MINIMUM_VERSION##
cmake_minimum_required(VERSION ##CMAKE_MINIMUM_VERSION## FATAL_ERROR)

###########
# Project #
###########

project(##PACKAGE_NAME##)

##########
# Target #
##########

add_library(##PACKAGE_NAME## INTERFACE)

#Add an alias so that library can be used inside the build tree
add_library(##ALIAS_PACKAGE_NAME## ALIAS ##PACKAGE_NAME##)
  
##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##
##TARGET_INCLUDE_DIRECTORIES##

#######################
# Target Dependencies #
#######################

##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##
##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##
