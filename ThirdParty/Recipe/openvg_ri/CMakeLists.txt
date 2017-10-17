# stop if cmake version below 3.2
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Custom make file that enables building of the openvg reference implementation on windows.
# project name
set(PROJECT_NAME libOpenVG)
set(EGL_PROJECT_NAME libEGL)
project(${PROJECT_NAME})

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")

set(PROJECT_PUBLIC_HEADER_DIRECTORIES 
    ri/include/EGL
    ri/include/VG
)

set(PROJECT_PUBLIC_FILES 
    license.txt
    readme.txt
)


# Add all the include and source files to the library
add_library(${PROJECT_NAME} SHARED
    ri/src/riApi.cpp
    ri/src/riContext.cpp
    ri/src/riFont.cpp
    ri/src/riImage.cpp
    ri/src/riMath.cpp
    ri/src/riMiniEGL.cpp
    ri/src/riPath.cpp
    ri/src/riPixelPipe.cpp
    ri/src/riRasterizer.cpp
    ri/src/riVGU.cpp
    ri/src/win32/riEGLOS.cpp  # Platform dependent
)

# Add all the include and source files to the library
add_library(${EGL_PROJECT_NAME} DummyEGL.cpp)


# Location of header files in this package
target_include_directories(${PROJECT_NAME} PUBLIC ri/include)
# The original project wants these defines too
target_include_directories(${PROJECT_NAME} PRIVATE 
    ri/include/EGL
    ri/include/VG
    ri/src
)

# Defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    _USRDLL
    OPENVG_DLL_EXPORTS
)


# 'make install' to the correct location
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
    ARCHIVE  DESTINATION "${INSTALL_LIB_DIR}"
    LIBRARY  DESTINATION "${INSTALL_LIB_DIR}"
    RUNTIME  DESTINATION "${INSTALL_BIN_DIR}")
install(DIRECTORY ${PROJECT_PUBLIC_HEADER_DIRECTORIES} DESTINATION "${INSTALL_INC_DIR}")
install(FILES ${PROJECT_PUBLIC_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}")

# 'make install' to the correct location
install(TARGETS ${EGL_PROJECT_NAME} EXPORT ${EGL_PROJECT_NAME}
    ARCHIVE  DESTINATION "${INSTALL_LIB_DIR}"
    LIBRARY  DESTINATION "${INSTALL_LIB_DIR}"
    RUNTIME  DESTINATION "${INSTALL_BIN_DIR}")
