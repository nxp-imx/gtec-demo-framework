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

# requires pkg-config, libwayland-dev, wayland-protocols

include(FindPkgConfig)

if (NOT DEFINED WAYLAND_PROTOCOLS_DIR)
	pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
endif()
if (NOT DEFINED WAYLAND_SCANNER)
	pkg_get_variable(WAYLAND_SCANNER wayland-scanner wayland_scanner)
endif()

set(XDG_GEN_OUTPUTS_DIR ${PROJECT_BINARY_DIR})

set(XDG_SHELL_PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)

set(XDG_SHELL_PUB_HEADERFILE include/df-xdg-shell-client-protocol.h)
set(XDG_SHELL_PRI_SOURCEFILE source/df-xdg-shell-protocol.c)

set(XDG_SHELL_PUB_HEADER ${XDG_GEN_OUTPUTS_DIR}/${XDG_SHELL_PUB_HEADERFILE})
set(XDG_SHELL_PRI_SOURCE ${XDG_GEN_OUTPUTS_DIR}/${XDG_SHELL_PRI_SOURCEFILE})

set(XDG_SHELL_OUTPUTS
    ${XDG_SHELL_PUB_HEADER}
    ${XDG_SHELL_PRI_SOURCE}
    )

set(XDG_DECORATION_PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml)

set(XDG_DECORATION_PUB_HEADERFILE include/df-xdg-decoration-client-protocol.h)
set(XDG_DECORATION_PRI_SOURCEFILE source/df-xdg-decoration-protocol.c)

set(XDG_DECORATION_PUB_HEADER ${XDG_GEN_OUTPUTS_DIR}/${XDG_DECORATION_PUB_HEADERFILE})
set(XDG_DECORATION_PRI_SOURCE ${XDG_GEN_OUTPUTS_DIR}/${XDG_DECORATION_PRI_SOURCEFILE})

set(XDG_DECORATION_OUTPUTS
    ${XDG_DECORATION_PUB_HEADER}
    ${XDG_DECORATION_PRI_SOURCE}
    )

set(XDG_INSTALL_HEADERS
    ${XDG_SHELL_PUB_HEADER} 
    ${XDG_DECORATION_PUB_HEADER}    
    )
    

if(NOT DEFINED WAYLAND_SCANNER)
    message(FATAL_ERROR "The wayland-scanner could not be found")
endif()

set(DF_WAYLAND_SCANNER ${WAYLAND_SCANNER})

message("-- Wayland protocol path: '" ${WAYLAND_PROTOCOLS_DIR} "'")
message("-- WaylandScanner path: '" ${WAYLAND_SCANNER} "'")
message("-- Patched WaylandScanner path: '" ${DF_WAYLAND_SCANNER} "'")
message("-- WaylandScanner protocol path: '" ${XDG_SHELL_PROTOCOL} "'")
message("-- WaylandScanner output path: '" ${XDG_GEN_OUTPUTS_DIR} "'")

add_custom_command(
    COMMAND ${DF_WAYLAND_SCANNER} ARGS client-header ${XDG_SHELL_PROTOCOL} ${XDG_SHELL_PUB_HEADERFILE}
    COMMAND ${DF_WAYLAND_SCANNER} ARGS public-code ${XDG_SHELL_PROTOCOL} ${XDG_SHELL_PRI_SOURCEFILE}
    DEPENDS ${XDG_SHELL_PROTOCOL}
    OUTPUT ${XDG_SHELL_OUTPUTS})

add_custom_command(
    COMMAND ${DF_WAYLAND_SCANNER} ARGS client-header ${XDG_DECORATION_PROTOCOL} ${XDG_DECORATION_PUB_HEADERFILE}
    COMMAND ${DF_WAYLAND_SCANNER} ARGS public-code ${XDG_DECORATION_PROTOCOL} ${XDG_DECORATION_PRI_SOURCEFILE}
    DEPENDS ${XDG_DECORATION_PROTOCOL}
    OUTPUT ${XDG_DECORATION_OUTPUTS})
        
add_custom_target(
    xdg-shell ALL
    DEPENDS ${XDG_SHELL_OUTPUTS} ${XDG_DECORATION_OUTPUTS})

include_directories(${XDG_GEN_OUTPUTS_DIR})

add_library(df-wayland-xdg-application-stable-latest STATIC ${XDG_SHELL_OUTPUTS} ${XDG_DECORATION_OUTPUTS})
set_target_properties(df-wayland-xdg-application-stable-latest PROPERTIES PUBLIC_HEADER "${XDG_INSTALL_HEADERS}")
target_include_directories(df-wayland-xdg-application-stable-latest PUBLIC ${XDG_GEN_OUTPUTS_DIR}/include)

install(TARGETS df-wayland-xdg-application-stable-latest PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
