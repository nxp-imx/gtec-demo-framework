# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindOpenGLES3
------------

Find OpenGL ES

IMPORTED Targets
^^^^^^^^^^^^^^^^

This module defines :prop_tgt:`IMPORTED` target ``OpenGLES3::OpenGLES3``, if
OpenGL ES 3 has been found.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables::

  OpenGLES3_FOUND          - "True" if OpenGLES was found
  OpenGLES3_INCLUDE_DIRS   - include directories for OpenGLES
  OpenGLES3_LIBRARIES      - link against this library to use OpenGLES

The module will also define cache variables::

  OpenGLES3_INCLUDE_DIR    - the OpenGLES include directory
  OpenGLES3_LIBRARY        - the path to the OpenGLES library
  OpenGLES3_VERSION_STRING - the OpenGLES version

#]=======================================================================]

set(_OpenGLES3_INCLUDE_SEARCH_PATHS "$ENV{FSL_GLES_INCLUDE_PATH} $ENV{KHRONOS_HEADERS}")
set(_OpenGLES3_LIB_SEARCH_PATHS "$ENV{FSL_GLES_LIB_PATH} $ENV{OPENGLES_LIBDIR}")

if(WIN32)
  #set(OpenGLES3_ANGLE_SDK_PATH "")
  set(OpenGLES3_ARM_SDK_PATH "c:/Program Files/ARM/Mali Developer Tools/Mali OpenGL ES Emulator 3.0.4-2-g8d905")
  set(OpenGLES3_VIVANTE_SDK_PATH "c:/Program Files (x86)/Vivante/vEmulator/x64")
  set(OpenGLES3_POWERVR_SDK_PATH "c:/Imagination/PowerVR_Graphics/PowerVR_SDK/SDK_2017_R1/Builds")
  set(OpenGLES3_ADRENO_SDK_PATH "c:/AdrenoSDK/Development")
  set(_OpenGLES3_LIB_SEARCH_NAMES "libGLESv2")
  set(_OpenGLES3_EGL_LIB_SEARCH_NAMES "libEGL")
else()
  set(_OpenGLES3_LIB_SEARCH_NAMES "GLESv2" "GLESv3" "OpenGLES")
  set(_OpenGLES3_EGL_LIB_SEARCH_NAMES "EGL")
endif()

# Add paths to search

# Angle
if(DEFINED OpenGLES3_ANGLE_SDK_PATH)
list(APPEND _OpenGLES3_INCLUDE_SEARCH_PATHS "${OpenGLES3_ANGLE_SDK_PATH}/include")
list(APPEND _OpenGLES3_LIB_SEARCH_PATHS "${OpenGLES3_ANGLE_SDK_PATH}/lib/Release")
if(WIN32)
list(APPEND _OpenGLES3_LIB_SEARCH_NAMES "libGLESv2.dll.lib")
list(APPEND _OpenGLES3_EGL_LIB_SEARCH_NAMES "libEGL.dll.lib")
endif()
endif()

# ARM Mali
if(DEFINED OpenGLES3_ARM_SDK_PATH)
list(APPEND _OpenGLES3_INCLUDE_SEARCH_PATHS "${OpenGLES3_ARM_SDK_PATH}/include")
list(APPEND _OpenGLES3_LIB_SEARCH_PATHS "${OpenGLES3_ARM_SDK_PATH}")
endif()

# Vivante
if(DEFINED OpenGLES3_VIVANTE_SDK_PATH)
list(APPEND _OpenGLES3_INCLUDE_SEARCH_PATHS "${OpenGLES3_VIVANTE_SDK_PATH}/inc")
list(APPEND _OpenGLES3_LIB_SEARCH_PATHS "${OpenGLES3_VIVANTE_SDK_PATH}/lib")
endif()

# PowerVR
if(DEFINED OpenGLES3_POWERVR_SDK_PATH)
list(APPEND _OpenGLES3_INCLUDE_SEARCH_PATHS "${OpenGLES3_POWERVR_SDK_PATH}/include")
list(APPEND _OpenGLES3_LIB_SEARCH_PATHS "${OpenGLES3_POWERVR_SDK_PATH}/Windows/x86_64/Lib")
endif()

# Adreno (Qualcomm)
if(DEFINED OpenGLES3_ADRENO_SDK_PATH)
list(APPEND _OpenGLES3_INCLUDE_SEARCH_PATHS "${OpenGLES3_ADRENO_SDK_PATH}/Inc")
list(APPEND _OpenGLES3_LIB_SEARCH_PATHS "${OpenGLES3_ADRENO_SDK_PATH}/Lib/x64")
endif()


#if(NOT WIN32)
list(APPEND _OpenGLES3_LIB_SEARCH_PATHS "opt/graphics/OpenGL/lib"
  "/usr/openwin/lib"
  "/usr/shlib"
  "/usr/X11R6/lib"
  "/opt/vc/lib"
  "/usr/lib/aarch64-linux-gnu"
  "/usr/lib/arm-linux-gnueabihf"
  "/usr/lib")
#endif


# Try to locate the OpenGL ES 3.2 header
find_path(OpenGLES32_INCLUDE_DIR
  NAMES GLES3/gl32.h
  PATHS
    ${_OpenGLES3_INCLUDE_SEARCH_PATHS}
  )

if(OpenGLES32_INCLUDE_DIR)
  # Try to locate the OpenGL ES 3.1 header, based on where we found the 3.2 header
  find_path(OpenGLES31_INCLUDE_DIR
    NAMES GLES3/gl31.h
    HINTS
      "${OpenGLES32_INCLUDE_DIR}"
    )

  # Try to locate the OpenGL ES 3 header, based on where we found the 3.2 header
  find_path(OpenGLES3_INCLUDE_DIR
    NAMES GLES3/gl3.h
    HINTS
      "${OpenGLES32_INCLUDE_DIR}"
    )
else()
  # Try to locate the OpenGL ES 3.1 header
  find_path(OpenGLES31_INCLUDE_DIR
    NAMES GLES3/gl31.h
    PATHS
      ${_OpenGLES3_INCLUDE_SEARCH_PATHS}
    )

  # Try to locate the OpenGL ES 3 header, based on where we found the 3.1 header
  if(OpenGLES31_INCLUDE_DIR)
    find_path(OpenGLES3_INCLUDE_DIR
      NAMES GLES3/gl3.h
      HINTS
        "${OpenGLES31_INCLUDE_DIR}"
      )
  else()
    # Try to locate the OpenGL ES 3 header
    find_path(OpenGLES3_INCLUDE_DIR
      NAMES GLES3/gl3.h
      PATHS
        ${_OpenGLES3_INCLUDE_SEARCH_PATHS}
      )
  endif()
endif()

find_library(OpenGLES3_LIBRARY
  NAMES ${_OpenGLES3_LIB_SEARCH_NAMES} 
  PATHS ${_OpenGLES3_LIB_SEARCH_PATHS})

if(OpenGLES32_INCLUDE_DIR)
  set(OpenGLES3_VERSION_STRING 3.2)
  set(OpenGLES3_INCLUDE_DIR ${OpenGLES32_INCLUDE_DIR})
elseif(OpenGLES31_INCLUDE_DIR)
  set(OpenGLES3_VERSION_STRING 3.1)
  set(OpenGLES3_INCLUDE_DIR ${OpenGLES31_INCLUDE_DIR})
elseif(OpenGLES30_INCLUDE_DIR)
  set(OpenGLES3_VERSION_STRING 3.0)
  set(OpenGLES3_INCLUDE_DIR ${OpenGLES30_INCLUDE_DIR})
endif()


# Find EGL headers
find_path(OpenGLES3_EGL_INCLUDE_DIR
  NAMES EGL/egl.h
  HINTS
    "${OpenGLES3_INCLUDE_DIR}")

# Find EGL library
find_library(OpenGLES3_EGL_LIBRARY
  NAMES ${_OpenGLES3_EGL_LIB_SEARCH_NAMES}
  HINTS ${OpenGLES3_LIBRARY}
  PATHS ${_OpenGLES3_LIB_SEARCH_PATHS})

if(NOT OpenGLES3_EGL_INCLUDE_DIR OR NOT OpenGLES3_EGL_LIBRARY)
  set(OpenGLES3_EGL_FOUND FALSE)
else()
  set(OpenGLES3_EGL_FOUND TRUE)
endif()

set(OpenGLES3_LIBRARIES ${OpenGLES3_LIBRARY})
set(OpenGLES3_INCLUDE_DIRS ${OpenGLES3_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpenGLES3
  FOUND_VAR OpenGLES3_FOUND
  REQUIRED_VARS OpenGLES3_INCLUDE_DIR OpenGLES3_LIBRARY OpenGLES3_EGL_INCLUDE_DIR OpenGLES3_EGL_LIBRARY
  VERSION_VAR OpenGLES3_VERSION_STRING)


mark_as_advanced(OpenGLES3_INCLUDE_DIR OpenGLES31_INCLUDE_DIR OpenGLES32_INCLUDE_DIR OpenGLES3_LIBRARY OpenGLES3_VERSION_STRING)

if(OpenGLES3_FOUND AND NOT TARGET OpenGLES3::OpenGLES3)
  add_library(OpenGLES3::OpenGLES3 UNKNOWN IMPORTED)
  set_target_properties(OpenGLES3::OpenGLES3 PROPERTIES
    IMPORTED_LOCATION "${OpenGLES3_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${OpenGLES3_INCLUDE_DIRS}")
endif()

if(TARGET OpenGLES3::OpenGLES3 AND OpenGLES3_EGL_FOUND AND NOT TARGET OpenGLES3::EGL)
  if(IS_ABSOLUTE "${OpenGLES3_EGL_LIBRARY}")
    add_library(OpenGLES3::EGL UNKNOWN IMPORTED)
    set_target_properties(OpenGLES3::EGL PROPERTIES IMPORTED_LOCATION
                          "${OpenGLES3_EGL_LIBRARY}")
  else()
    add_library(OpenGLES3::EGL INTERFACE IMPORTED)
    set_target_properties(OpenGLES3::EGL PROPERTIES IMPORTED_LIBNAME
                          "${OpenGLES3_EGL_LIBRARY}")
  endif()
  set_target_properties(OpenGLES3::EGL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                        "${OpenGLES3_EGL_INCLUDE_DIR}")

  # Ensure OpenGLES3::EGL is a dependency
  set_target_properties(OpenGLES3::OpenGLES3 PROPERTIES INTERFACE_LINK_LIBRARIES
                        OpenGLES3::EGL)
endif()
