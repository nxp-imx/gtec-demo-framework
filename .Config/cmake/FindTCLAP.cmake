# - Try to find tclap
# Once done this will define
#  TCLAP_FOUND - System has tclap
#  TCLAP_INCLUDE_DIRS - The tclap include directories
#  TCLAP_DEFINITIONS - Compiler switches required for using tclap

#find_package(PkgConfig)
#pkg_check_modules(PC_TCLAP QUIET tclap)
#set(YAMLCPP_DEFINITIONS ${PC_TCLAP_CFLAGS_OTHER})

if(NOT TCLAP_INCLUDE_DIR)
  set(TCLAP_INCLUDE_DIR "$(FSL_GRAPHICS_SDK)/ThirdParty/tclap-1.2.1/include")
endif()

find_path(TCLAP_INCLUDE_DIR tclap/CmdLine.h
          HINTS ${PC_TCLAP_INCLUDEDIR} ${PC_TCLAP_INCLUDE_DIRS} )

#set(TCLAP_INCLUDE_DIRS ${TCLAP_INCLUDE_DIR} )

add_library(Tclap_qw, INTERFACE)

target_include_directories(Tclap
    PUBLIC ${TCLAP_INCLUDE_DIR}
    )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set TCLAP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(TCLAP  DEFAULT_MSG
                                  TCLAP_INCLUDE_DIR)

mark_as_advanced(TCLAP_INCLUDE_DIR)