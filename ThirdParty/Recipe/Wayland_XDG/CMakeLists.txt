# stop if cmake version below 3.10.2
cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(
  Wayland_XDG_DF
  VERSION 1.0.0
  LANGUAGES C
)

# Generate the client header and glue library from xdg-shell.xml. 
# The generated header and library is 100% only dependent on Wayland.
# The way this is implemented means that we always use the latest supplied xdg-shell version.
# Unfortunately wayland does not provide a semantic verioned header and library so we have to generate it for them which 
# is less than ideal. This also shifts the burden of maintaining a more complex build script to all end users of Wayland.

include(FindPkgConfig)

if (NOT DEFINED WAYLAND_PROTOCOLS_DIR)
	pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
endif()
if (NOT DEFINED WAYLAND_SCANNER)
	pkg_get_variable(WAYLAND_SCANNER wayland-scanner wayland_scanner)
endif()


if(NOT DEFINED WAYLAND_SCANNER)
    message(FATAL_ERROR "The wayland-scanner could not be found")
endif()

if(NOT DEFINED ENV{FSL_HOST_ROOTFS})
    set(DF_WAYLAND_SCANNER ${WAYLAND_SCANNER})
else()
    # For yocto-sdk builds we need to be sure we utilize the supplied wayland scanner and not the host version
    set(DF_WAYLAND_SCANNER $ENV{FSL_HOST_ROOTFS}${WAYLAND_SCANNER})
endif()

message("-- WAYLAND_PROTOCOLS_DIR: '" ${WAYLAND_PROTOCOLS_DIR} "'")
message("-- WAYLAND_SCANNER:       '" ${WAYLAND_SCANNER} "'")
message("-- DF_WAYLAND_SCANNER:    '" ${DF_WAYLAND_SCANNER} "'")

set(XDG_SHELL_PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)
set(XDG_SHELL_GEN_OUTPUTS_DIR ${PROJECT_BINARY_DIR})
set(XDG_SHELL_OUTPUTS
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg-shell-client-protocol.h
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg-shell-protocol.c
    )


add_custom_command(
    COMMAND ${DF_WAYLAND_SCANNER} ARGS client-header ${XDG_SHELL_PROTOCOL} df-xdg-shell-client-protocol.h
    COMMAND ${DF_WAYLAND_SCANNER} ARGS public-code ${XDG_SHELL_PROTOCOL} df-xdg-shell-protocol.c
    DEPENDS ${XDG_SHELL_PROTOCOL}
    OUTPUT ${XDG_SHELL_OUTPUTS})

add_custom_target(
    xdg-shell ALL
    DEPENDS ${XDG_SHELL_OUTPUTS})

set(XDG_SHELL_SRCS
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg-shell-client-protocol.h
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg-shell-protocol.c
    )
include_directories(${XDG_SHELL_GEN_OUTPUTS_DIR})

add_library(df-wayland-xdg-application-stable-latest STATIC ${XDG_SHELL_SRCS})
target_include_directories(df-wayland-xdg-application-stable-latest PUBLIC ${XDG_SHELL_GEN_OUTPUTS_DIR})
target_sources(df-wayland-xdg-application-stable-latest PUBLIC FILE_SET HEADERS BASE_DIRS ${XDG_SHELL_GEN_OUTPUTS_DIR} FILES ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg-shell-client-protocol.h)

install(TARGETS df-wayland-xdg-application-stable-latest FILE_SET HEADERS)
