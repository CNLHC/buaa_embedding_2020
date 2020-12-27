cmake_minimum_required(VERSION 3.10.2)
project(buaa_embedding_exp6)

set(MODULE_NAME myzr_zlg7290)

set(kerneldir "" CACHE STRING "Path to the kernel build directory")


find_file(kernel_makefile NAMES Makefile
                          PATHS ${kerneldir} NO_DEFAULT_PATH)
if(NOT kernel_makefile)
  message(FATAL_ERROR "There is no Makefile in kerneldir!")
endif()


#add_library(buaa_led_lib STATIC led_ioctl.c)
#get_target_property(module_sources buaa_led_lib SOURCES)
list(APPEND module_sources ./myzr_zlg7290.c ./led_ioctl.h)
string(REPLACE ";" " " module_sources_string "${module_sources}")

configure_file(Kbuild.in Kbuild @ONLY)

foreach(src ${module_sources})
      configure_file(${src} ${src} COPYONLY)
endforeach()


set(module_cmd ${CMAKE_MAKE_PROGRAM} -C ${kerneldir} M=${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(OUTPUT ${MODULE_NAME}.ko
  COMMAND ${module_cmd} modules
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${module_sources} ${CMAKE_CURRENT_BINARY_DIR}/Kbuild
  VERBATIM)

add_custom_target(module DEPENDS ${MODULE_NAME}.ko)
add_custom_target(module-clean COMMAND ${module_cmd} clean)

add_executable(clock clock.c)
target_include_directories(clock PUBLIC .)


