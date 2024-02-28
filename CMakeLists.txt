cmake_minimum_required(VERSION 3.24)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message(STATUS "Found CCACHE!")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(PROJECT_NAME esp_flasher_stm)

project(${PROJECT_NAME} LANGUAGES C CXX ASM)

# To enable colored terminal
option(FORCE_COLORED_OUTPUT
       "Always produce ANSI-colored output (GNU/Clang only.)" TRUE)
if(${FORCE_COLORED_OUTPUT})
  if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
  endif()
endif()

# Reduce binary size. Might be used to code to fit in the MCU
option(GARBAGE_COLLECT_SECTIONS
       "Use -f{function,data}-sections and -Wl,--gc-sections." TRUE)
if(${GARBAGE_COLLECT_SECTIONS})
  add_compile_options(-ffunction-sections -fdata-sections)
  add_link_options(-Wl,--gc-sections)
endif()

add_subdirectory(Modules)

# add_subdirectory(Espressif)
add_library(
  Espressif STATIC
  Espressif/src/esp_targets.c Espressif/src/md5_hash.c
  Espressif/src/esp_loader.c Espressif/src/protocol_common.c
  Espressif/src/protocol_uart.c Espressif/src/slip.c)

target_include_directories(
  Espressif
  PUBLIC Espressif/include Espressif/port
  PRIVATE Espressif/private_include)

target_compile_definitions(
  Espressif
  PUBLIC MD5_ENABLED=1 SERIAL_FLASHER_INTERFACE_UART=1
          SERIAL_FLASHER_DEBUG_TRACE=1 SERIAL_FLASHER_WRITE_BLOCK_RETRIES=3)

add_subdirectory(Firmware)
