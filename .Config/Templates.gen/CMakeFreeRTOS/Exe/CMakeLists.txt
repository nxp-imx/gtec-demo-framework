##COMMON_HEADER##

###########
# Modules #
###########

##SNIPPET_COMMON_MODULES##

###########
# Options #
###########

##SNIPPET_OPTIONS_EXE##

#############
# PROJECT
#############

project(
  ##PACKAGE_NAME##
  VERSION ##PROJECT_VERSION##
  LANGUAGES CXX
)

##PROJECT_PREVENT_IN_SOURCE_BUILDS##

# Add all the include and source files to the library
add_executable(##PACKAGE_NAME####PACKAGE_SOURCE_FILES####PACKAGE_PUBLIC_INCLUDE_FILES####PACKAGE_PRIVATE_INCLUDE_FILES##
)

#Add an alias so that library can be used inside the build tree
add_executable(##ALIAS_PACKAGE_NAME## ALIAS ##PACKAGE_NAME##)

##PACKAGES_FIND_DIRECT_EXTERNAL_DEPENDENCIES##
##TARGET_INCLUDE_DIRECTORIES##

################
# Dependencies #
################

##PACKAGE_DIRECT_DEPENDENCIES_TARGET_LINK_LIBRARIES##

target_link_libraries (##PACKAGE_NAME## PRIVATE FreeRTOS_LIB)

### FreeRTOS magic section Begin

TARGET_LINK_LIBRARIES(##PACKAGE_NAME## PRIVATE -Wl,--start-group)
target_link_libraries(##PACKAGE_NAME## PRIVATE debug m)
target_link_libraries(##PACKAGE_NAME## PRIVATE debug c)
target_link_libraries(##PACKAGE_NAME## PRIVATE debug gcc)
target_link_libraries(##PACKAGE_NAME## PRIVATE debug nosys)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized m)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized c)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized gcc)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized nosys)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized m)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized c)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized gcc)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized nosys)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized m)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized c)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized gcc)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized nosys)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized m)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized c)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized gcc)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized nosys)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized m)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized c)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized gcc)
target_link_libraries(##PACKAGE_NAME## PRIVATE optimized nosys)
TARGET_LINK_LIBRARIES(##PACKAGE_NAME## PRIVATE -Wl,--end-group)

ADD_CUSTOM_COMMAND(TARGET ##PACKAGE_NAME## POST_BUILD COMMAND ${CMAKE_OBJCOPY}
-Obinary ${EXECUTABLE_OUTPUT_PATH}/##PACKAGE_NAME## ${EXECUTABLE_OUTPUT_PATH}/m4_image.bin)

### FreeRTOS magic section End


##PACKAGE_DIRECT_DEPENDENCIES_TARGET_COMPILE_DEFINITIONS##
##PACKAGE_COPY_FILES##
##SNIPPET_DEFAULT_TARGET_COMPILE_FEATURES##
##SNIPPET_DEFAULT_TARGET_COMPILE_OPTIONS##

# Syncronize the content directory
#add_custom_command(
#    TARGET ##PACKAGE_NAME##
#    POST_BUILD
#    COMMAND ENV$(FSL_GRAPHICS_SDK)/.Config/FslContentSync.py ##PACKAGE_PATH##/Content $(DESTDIR)/Content --create --cache --ifExist
#)
set_property(TARGET ##PACKAGE_NAME## PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "##PACKAGE_PATH##")


if(INSTALL_EXE)
##PACKAGE_GENERATE_INSTALL_INSTRUCTIONS##
endif()
