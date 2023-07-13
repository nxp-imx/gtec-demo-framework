cmake_minimum_required(VERSION ##CMAKE_MINIMUM_VERSION## FATAL_ERROR)

project(DemoFramework)

##CMAKE_DEFINE_PACKAGE_ROOT_VARIABLES##

# location of additional cmake modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${FSL_GRAPHICS_SDK}/.Config/cmake
    )

set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

# Disabled due to win7 x64 problems
# find python
#find_package(PythonInterp)
#if(NOT PYTHONINTERP_FOUND)
#    message(FATAL_ERROR "ERROR: Python interpreter not found. Cannot continue - sorry.")
#endif()

find_package(game-activity REQUIRED CONFIG)

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

##CMAKE_PACKAGE_FIND_DIRECT_EXTERNAL_DEPENDENCIES##
##CMAKE_PACKAGE_DIRECT_DEPENDENCIES_ADD_SUBDIRECTORIES##

##CMAKE_THIRD_PARTY_ADD_LIBS##

# Add the 'android executable' shared lib
add_library(DemoNativeActivity SHARED DemoNativeActivity.cpp )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(DemoNativeActivity PUBLIC android game-activity::game-activity_static ${log-lib} z ##CMAKE_THIRD_PARTY_LIBS## ##CMAKE_PACKAGE_EXE_LIB##
                      PRIVATE DemoFramework::FslDemoPlatform.Link)


#set(FslDemoApp.OpenGLES2_DIR "e:/Work/DemoFramework/DemoFramework/FslDemoApp.OpenGLES2")
#find_package(FslDemoApp.OpenGLES2 REQUIRED)
#target_link_libraries(DemoGameActivity PUBLIC app-glue ${log-lib} DepFslDemoApp.OpenGLES2)
