cmake_minimum_required(VERSION 3.24)

project(esp_flasher_stm LANGUAGES C CXX ASM)

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

add_subdirectory(Espressif)
