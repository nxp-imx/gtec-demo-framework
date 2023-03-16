# Generate the client header and glue library from xdg-shell.xml

include(FindPkgConfig)

if (NOT DEFINED WAYLAND_PROTOCOLS_DIR)
	pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
endif()
if (NOT DEFINED WAYLAND_SCANNER)
	pkg_get_variable(WAYLAND_SCANNER wayland-scanner wayland_scanner)
endif()

set(XDG_SHELL_PROTOCOL ${WAYLAND_PROTOCOLS_DIR}/stable/xdg-shell/xdg-shell.xml)
set(XDG_SHELL_GEN_OUTPUTS_DIR ${PROJECT_BINARY_DIR})
set(XDG_SHELL_OUTPUTS
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg/xdg-shell-client-protocol.h
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/xdg-shell-protocol.c
    )

add_custom_command(
    COMMAND ${WAYLAND_SCANNER} ARGS client-header ${XDG_SHELL_PROTOCOL} df-xdg/xdg-shell-client-protocol.h
    COMMAND ${WAYLAND_SCANNER} ARGS public-code ${XDG_SHELL_PROTOCOL} xdg-shell-protocol.c
    DEPENDS ${XDG_SHELL_PROTOCOL}
    OUTPUT ${XDG_SHELL_OUTPUTS})

add_custom_target(
    xdg-shell ALL
    DEPENDS ${XDG_SHELL_OUTPUTS})

set(XDG_SHELL_SRCS
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg/xdg-shell-client-protocol.h
    ${XDG_SHELL_GEN_OUTPUTS_DIR}/xdg-shell-protocol.c
    )
include_directories(${XDG_SHELL_GEN_OUTPUTS_DIR})

add_library(df-wayland-xdg-application-stable-latest STATIC ${XDG_SHELL_SRCS})
target_include_directories(df-wayland-xdg-application-stable-latest PUBLIC ${XDG_SHELL_GEN_OUTPUTS_DIR})
target_sources(df-wayland-xdg-application-stable-latest PUBLIC FILE_SET HEADERS BASE_DIRS ${XDG_SHELL_GEN_OUTPUTS_DIR} FILES ${XDG_SHELL_GEN_OUTPUTS_DIR}/df-xdg/xdg-shell-client-protocol.h)

install(TARGETS df-wayland-xdg-application-stable-latest FILE_SET HEADERS)
