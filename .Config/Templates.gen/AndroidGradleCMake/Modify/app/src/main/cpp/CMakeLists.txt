cmake_minimum_required(VERSION ##CMAKE_MINIMUM_VERSION## FATAL_ERROR)

project(DemoFramework)

##CMAKE_DEFINE_PACKAGE_ROOT_VARIABLES##

# location of additional cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${FSL_GRAPHICS_SDK}/.Config/cmake
    )

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

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
# variable. Because CMake includes system libraries in the search path by
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

##CMAKE_THIRD_PARTY_ADD_LIBS##

# Add the 'android executable' shared lib
add_library(DemoNativeActivity SHARED DemoNativeActivity.cpp )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(DemoNativeActivity PUBLIC android app-glue ndk_helper ${log-lib} z ##CMAKE_THIRD_PARTY_LIBS## ##CMAKE_PACKAGE_EXE_LIB## 
                      PRIVATE DemoFramework::FslDemoPlatform.Link)


#set(FslDemoApp.OpenGLES2_DIR "e:/Work/DemoFramework/DemoFramework/FslDemoApp.OpenGLES2")
#find_package(FslDemoApp.OpenGLES2 REQUIRED)
#target_link_libraries(DemoNativeActivity PUBLIC app-glue ${log-lib} DepFslDemoApp.OpenGLES2)
