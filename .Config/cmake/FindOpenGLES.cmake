# Find OpenGLES
#
# OpenGLES_INCLUDE_DIRS
# OpenGLES_LIBRARY
# OpenGLES_FOUND
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  if (WIN32)
      find_path(OpenGLES_INCLUDE_DIRS NAMES GLES2/gl2.h HINTS
          "$ENV{KHRONOS_HEADERS}")
      find_library(OpenGLES_LIBRARY NAMES libGLESv2.lib HINTS
          "$ENV{KHRONOS_HEADERS}/..")
  else()
      find_path(OpenGLES_INCLUDE_DIRS NAMES GLES2/gl2.h HINTS
                /usr/openwin/share/include
                /opt/graphics/OpenGL/include
                /opt/vc/include
                /usr/X11R6/include
                /usr/include)
      find_library(OpenGLES_LIBRARY NAMES GLESv2 PATHS
                   /opt/graphics/OpenGL/lib
                   /usr/openwin/lib
                   /usr/shlib /usr/X11R6/lib
                   /opt/vc/lib
                   /usr/lib/aarch64-linux-gnu
                   /usr/lib/arm-linux-gnueabihf
                   /usr/lib)  
  endif()

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(OpenGLES DEFAULT_MSG OpenGLES_LIBRARY OpenGLES_INCLUDE_DIRS)
else()
  set(OpenGLES_INCLUDE_DIRS)
  set(OpenGLES_LIBRARY, "GLESv2")
  set(OpenGLES_FOUND TRUE)
endif()

mark_as_advanced(OpenGLES_INCLUDE_DIRS OpenGLES_LIBRARY)