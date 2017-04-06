# Find EGL
#
# EGL_INCLUDE_DIRS
# EGL_LIBRARY
# EGL_FOUND

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  if (WIN32)
      find_path(EGL_INCLUDE_DIRS NAMES EGL/egl.h HINTS
          "$ENV{KHRONOS_HEADERS}")
      find_library(EGL_LIBRARY NAMES libEGL.lib HINTS
          "$ENV{KHRONOS_HEADERS}/..")
  else()
      find_path(EGL_INCLUDE_DIR EGL/egl.h PATHS 
                /usr/openwin/share/include
                /opt/graphics/OpenGL/include
                /opt/vc/include
                /usr/X11R6/include
                /usr/include)
      find_library(EGL_LIBRARY NAMES EGL PATHS 
                   /opt/graphics/OpenGL/lib
                   /usr/openwin/lib
                   /usr/shlib
                   /usr/X11R6/lib
                   /opt/vc/lib
                   /usr/lib/aarch64-linux-gnu
                   /usr/lib/arm-linux-gnueabihf
                   /usr/lib)
  endif()
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(EGL DEFAULT_MSG EGL_LIBRARY EGL_INCLUDE_DIRS)
else()
  set(EGL_INCLUDE_DIRS)
  set(EGL_LIBRARY libGLESv2.so)
  set(EGL_FOUND TRUE)
endif()
mark_as_advanced(EGL_INCLUDE_DIRS EGL_LIBRARY)