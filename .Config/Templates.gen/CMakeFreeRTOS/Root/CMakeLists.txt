INCLUDE(CMakeForceCompiler)

##COMMON_HEADER##

##########
# CHECKS #
##########

#if(NOT DEFINED ENV{FSL_GRAPHICS_SDK})
#    message(FATAL_ERROR "The FSL_GRAPHICS_SDK environment variable is not defined")
#    # some more commands
#else()
#  set(FSL_GRAPHICS_SDK "$ENV{FSL_GRAPHICS_SDK}")
#  string(REPLACE "\\" "/" FSL_GRAPHICS_SDK ${FSL_GRAPHICS_SDK})
#endif()

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

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-hack/Platform/cmake")

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

SET (CMAKE_CXX_STANDARD 14)

INCLUDE(LegacySetup)
INCLUDE(LegacyConfigFlags)
INCLUDE(LegacyConfigFlags2)


#######################
# Modules and scripts #
#######################

# Overload find_package as recommended by "Effective CMake" to keep all 'dependencies' the same.
set(findpackage_as_subproject
  ##ALL_PACKAGE_NAMES##
)

macro(find_package)
  if(NOT "${ARGV0}" IN_LIST findpackage_as_subproject)
    _find_package(${ARGV})
  endif()
endmacro()

###########
# Options #
###########

##SNIPPET_OPTIONS_EXE##
##SNIPPET_OPTIONS_LIB##

#############
# PROJECT
#############

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

# FIX: FreeRTOS - Add the CMake platform adatpon library hack
add_subdirectory(FreeRTOS-hack/Platform)

##PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##
