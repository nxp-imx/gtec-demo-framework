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
##SNIPPET_OPTIONS_EXE##

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

##PROJECT_PREVENT_IN_SOURCE_BUILDS####PACKAGE_CONTENTSECTION_OUTPUT####PACKAGE_CONTENTBUILDER####PACKAGE_CONTENTDEP##

##########
# Target #
##########

# Add all the include and source files to the library
add_executable(##PACKAGE_NAME####PACKAGE_SOURCE_FILES####PACKAGE_PUBLIC_INCLUDE_FILES####PACKAGE_PRIVATE_INCLUDE_FILES####PACKAGE_CONTENTDEP_OUTPUT_FILES##
)

# Add an alias so that library can be used inside the build tree
add_executable(##ALIAS_PACKAGE_NAME## ALIAS ##PACKAGE_NAME##)

# Ensure that debug exes are postfixed with a _d
set_property(TARGET ##PACKAGE_NAME## PROPERTY DEBUG_POSTFIX _d)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ##PACKAGE_NAME##)

file(GENERATE 
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/.fsl-build/config_$<CONFIG>.json
  CONTENT {"EXE_PATH":"$<TARGET_FILE:##PACKAGE_NAME##>"}
)

##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##
##TARGET_INCLUDE_DIRECTORIES##

#########################
# SOURCE GROUPS FOR IDE #
#########################
  
##PACKAGE_SOURCE_GROUP##

#######################
# Target Dependencies #
#######################

# Unfortunately some platforms do not add pthreads automatically when it detects the usage of std::thread and therefore
# hands over the burden of a internal implementation detail to the users of a standard library feature.
# Hence we need to know it uses pthreads internally and remember to add it.. 
# This causes unnecessary build system complications that affect thousands of projects :(
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package (Threads REQUIRED)

##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##

# FIX: this should be driven by package dependencies instead
if (NOT("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC"))
  target_link_libraries (##PACKAGE_NAME## PRIVATE Threads::Threads)
endif()

##PACKAGE_COMPILER_SPECIFIC_FILE_DEPENDENCIES##
##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##
##PACKAGE_COPY_FILES##
##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##
##SNIPPET_DEFAULT_TARGET_COMPILE_OPTIONS##
##PACKAGE_VARIANT_SETTINGS##
# Sets the visual studio working directory to point to the in source package directory
# This is not needed anymore.
#set_property(TARGET ##PACKAGE_NAME## PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "##PACKAGE_PATH##")
##PACKAGE_TARGET_SPECIAL_FILES##

if(INSTALL_EXE)
##PACKAGE_GENERATE_INSTALL_INSTRUCTIONS##
endif()

##PACKAGE_EMSCRIPTEN##
