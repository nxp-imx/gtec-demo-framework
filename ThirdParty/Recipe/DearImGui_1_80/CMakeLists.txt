# stop if cmake version below 3.10.2
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

#################################################################################################################
## Custom CMAKE build script for DearImGui since the official distribution does not come with any build scripts!
#################################################################################################################

# Generate folders for IDE's
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


###########
# Options #
###########

option(INSTALL_LIB "Install all libs" ON)

###########
# Project #
###########

project(
  DearImGui
  VERSION 1.80.0
  LANGUAGES CXX
)

# Disable project in-source buils
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not permitted. Use a separate folder for building. Unfortunately some files might have been generated by cmake in this directory, please delete those before continuing.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")


##########
# Target #
##########

set(DearImGui_SOURCE 
  source/imgui.cpp
  source/imgui_demo.cpp
  source/imgui_draw.cpp
  source/imgui_tables.cpp
  source/imgui_widgets.cpp
)

set(DearImGui_INCLUDE 
  include/imconfig.h
  include/imgui.h
  include/imgui_internal.h
  include/imstb_rectpack.h
  include/imstb_textedit.h
  include/imstb_truetype.h
)


# Add all the include and source files to the library
add_library(DearImGui ${DearImGui_SOURCE} ${DearImGui_INCLUDE}
)

#Add an alias so that library can be used inside the build tree
add_library(DearImGui::DearImGui ALIAS DearImGui)

# Ensure that debug libs are postfixed with a _d
set_property(TARGET DearImGui PROPERTY DEBUG_POSTFIX _d)

# Add libs folder for target project
set_property(TARGET DearImGui PROPERTY FOLDER "Libs")
 

# Location of header files in this package
target_include_directories(DearImGui
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<INSTALL_INTERFACE:source>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>
)

target_compile_features(DearImGui
  PUBLIC
    cxx_std_14 # Enable C++14
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(DearImGui PRIVATE -W -Wall -Wtype-limits -Wuninitialized)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
target_compile_options(DearImGui PRIVATE -Wall -Wempty-body -Wmissing-field-initializers -Wtype-limits -Wuninitialized)
target_link_libraries(DearImGui PRIVATE optimized -s)
if(CODE_COVERAGE)
  set( 
      COVERAGE_GNU_DEBUG_OPTIONS
      -fprofile-arcs -ftest-coverage -g -O0
     )
  target_compile_options(DearImGui PRIVATE "$<$<CONFIG:DEBUG>:${COVERAGE_GNU_DEBUG_OPTIONS}>")
  target_link_libraries(DearImGui PRIVATE debug gcov)
endif()
#elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(DearImGui PRIVATE /W3 /MP)
endif()


if (MSVC)
  option(VS_ADD_NATIVE_VISUALIZERS "Configure project to use Visual Studio native visualizers" TRUE)
else()
  set(VS_ADD_NATIVE_VISUALIZERS FALSE CACHE INTERNAL "Native visualizers are Visual Studio extension" FORCE)
endif()


if(INSTALL_LIB)
#############################
# Installation instructions #
#############################

include(GNUInstallDirs)

#file(RELATIVE_PATH relDir
#${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
#${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
#set(CMAKE_INSTALL_RPATH $ORIGIN $ORIGIN/${relDir})

set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/DearImGui)

install(TARGETS DearImGui
  EXPORT DearImGuiTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/DearImGuiConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY ExactVersion
)

# Export the targets to a script
install(EXPORT DearImGuiTargets
  FILE DearImGuiTargets.cmake
  NAMESPACE DearImGui::
  DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the config, configversion and custom find modules
# FIX: configure_file(DearImGuiConfig.cmake.in DearImGuiConfig.cmake @ONLY)
install(
  FILES
# FIX:   "${CMAKE_CURRENT_BINARY_DIR}/DearImGuiConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DearImGuiConfigVersion.cmake"
  DESTINATION ${INSTALL_CONFIGDIR}
)

#################################
# Exporting from the build tree #
#################################

#export(
#  TARGETS
#    DearImGuiTargets
#  NAMESPACE DearImGui::
#  FILE ${CMAKE_CURRENT_BINARY_DIR}/DearImGuiTargets.cmake
#)

# Register package in user's package registry
#export(PACKAGE DearImGui)

endif()
