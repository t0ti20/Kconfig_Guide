cmake_minimum_required(VERSION 3.16)
project(KconfigDemo CXX)

set(CMAKE_CXX_STANDARD 17)

# ─── Paths ────────────────────────────────────────────────────────────────────
set(KCONFIG_ROOT  "${CMAKE_SOURCE_DIR}/Kconfig")
set(DOTCONFIG     "${CMAKE_SOURCE_DIR}/.config")
set(AUTOCONF_DIR  "${CMAKE_BINARY_DIR}/include/generated")
set(AUTOCONF_H    "${AUTOCONF_DIR}/autoconf.h")

# ─── Bootstrap: auto-create .config if it does not exist yet ─────────────────
if(NOT EXISTS "${DOTCONFIG}")
    message(STATUS "[Kconfig] No .config found — generating defaults with alldefconfig")
    execute_process(
        COMMAND alldefconfig "${KCONFIG_ROOT}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _rc
    )
    if(NOT _rc EQUAL 0)
        message(FATAL_ERROR
            "[Kconfig] alldefconfig failed (exit ${_rc}).\n"
            "Make sure kconfiglib is installed:  pip install kconfiglib")
    endif()
endif()

# ─── Target: menuconfig ───────────────────────────────────────────────────────
add_custom_target(menuconfig
    COMMAND menuconfig "${KCONFIG_ROOT}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    USES_TERMINAL
    COMMENT "[Kconfig] Opening menuconfig — save and exit when done, then rebuild"
)

# ─── Command: .config → autoconf.h ───────────────────────────────────────────
add_custom_command(
    OUTPUT  "${AUTOCONF_H}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${AUTOCONF_DIR}"
    COMMAND genconfig
                --header-path "${AUTOCONF_H}"
                "${KCONFIG_ROOT}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    DEPENDS "${DOTCONFIG}" "${KCONFIG_ROOT}"
    COMMENT "[Kconfig] Generating autoconf.h from .config"
)

add_custom_target(kconfig_headers DEPENDS "${AUTOCONF_H}")

# ─── Executable ───────────────────────────────────────────────────────────────
add_executable(app src/main.cpp)
add_dependencies(app kconfig_headers)
target_include_directories(app PRIVATE "${AUTOCONF_DIR}")
