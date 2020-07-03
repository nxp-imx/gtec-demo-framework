##COMMON_HEADER##

message(STATUS "Using CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

##########
# CHECKS #
##########

##DEFINE_PATH_ENVIRONMENT_AS_VARIABLES##

# location of additional cmake modules
#set(CMAKE_MODULE_PATH
#    ${CMAKE_MODULE_PATH}
#    ${FSL_GRAPHICS_SDK}/.Config/cmake
#    )

# Disabled due to win7 x64 problems
# find python
#find_package(PythonInterp)
#if(NOT PYTHONINTERP_FOUND)
#    message(FATAL_ERROR "ERROR: Python interpreter not found. Cannot continue - sorry.")
#endif()

#######################
# Modules and scripts #
#######################

if(NOT DEFINED FSL_FIND_PACKAGE)
# Overload find_package as recommended by "Effective CMake" to keep all 'dependencies' the same.
set(findpackage_as_subproject
  ##ALL_PACKAGE_NAMES##
)

macro(find_package)
  if(NOT "${ARGV0}" IN_LIST findpackage_as_subproject)
    _find_package(${ARGV})
  endif()
endmacro()
set(FSL_FIND_PACKAGE 1)
endif()

###########
# Options #
###########

##SNIPPET_OPTIONS_EXE##
##SNIPPET_OPTIONS_LIB##

#########
# Cache #
#########

##SNIPPET_CACHE_VARIANTS##

###########
# Project #
###########

project(
  ##PROJECT_NAME##
  VERSION ##PROJECT_VERSION##
  LANGUAGES CXX
)

##PROJECT_PREVENT_IN_SOURCE_BUILDS##

#########################
# EXTERNAL DEPENDENCIES #
#########################

##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##

#########################
# INTERNAL DEPENDENCIES #
#########################

##PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES####EXTENDED_PROJECT_PARENT##