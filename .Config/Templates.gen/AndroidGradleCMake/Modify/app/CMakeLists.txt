cmake_minimum_required(VERSION 3.4.1 FATAL_ERROR)


if(NOT DEFINED ENV{FSL_GRAPHICS_SDK})
    message(FATAL_ERROR "The FSL_GRAPHICS_SDK environment variable is not defined")
    # some more commands
else()
  set(FSL_GRAPHICS_SDK "$ENV{FSL_GRAPHICS_SDK}")
  string(REPLACE "\\" "/" FSL_GRAPHICS_SDK ${FSL_GRAPHICS_SDK})
endif()

# location of additional cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${FSL_GRAPHICS_SDK}/.Config/cmake
    )

# Disabled due to win7 x64 problems
# find python
#find_package(PythonInterp)
#if(NOT PYTHONINTERP_FOUND)
#    message(FATAL_ERROR "ERROR: Python interpreter not found. Cannot continue - sorry.")
#endif()

# Setting this just gives stranger errors
#if(CMAKE_HOST_WIN32)
#  # default is normally 250
#  set(CMAKE_OBJECT_PATH_MAX 260)
#  set(CMAKE_OBJECT_NAME_MAX 128)
#endif()

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# Pull in the log lib
find_library(log-lib log)

# Pull in the app glue and stup the include dirs for it              
add_library(app-glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.h)
add_library(FslAndroidDemoApp::app-glue ALIAS app-glue)

#target_include_directories(app-glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

add_library(ndk_helper STATIC
            ${ANDROID_NDK}/sources/android/ndk_helper/gestureDetector.cpp
            ${ANDROID_NDK}/sources/android/ndk_helper/JNIHelper.cpp
            ${ANDROID_NDK}/sources/android/ndk_helper/perfMonitor.cpp
            )
add_library(FslAndroidDemoApp::ndk_helper ALIAS ndk_helper)

include_directories(${ANDROID_NDK}/sources/android/ndk_helper)


##CMAKE_PACKAGE_FIND_DIRECT_EXTERNAL_DEPENDENCIES##
##CMAKE_PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##

##CMAKE_ASSIMP_SUBDIR##

# Add the 'android executable' shared lib
add_library(DemoNativeActivity SHARED src/main/cpp/DemoNativeActivity.cpp )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.
target_link_libraries(DemoNativeActivity PUBLIC android app-glue ndk_helper ${log-lib} ##CMAKE_ASSIMP_LIB## ##CMAKE_PACKAGE_EXE_LIB##)



#set(FslDemoAppGLES2_DIR "e:/Work/DemoFramework/DemoFramework/FslDemoAppGLES2")
#find_package(FslDemoAppGLES2 REQUIRED)
#target_link_libraries(DemoNativeActivity PUBLIC app-glue ${log-lib} DepFslDemoAppGLES2)
