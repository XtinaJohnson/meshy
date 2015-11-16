# Minimum CMake version.
cmake_minimum_required (VERSION 2.8.5)

# Project and version numbers.
project (meshy)

# Look for Polymec and use its CMake settings.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${POLYMEC_PREFIX}/share/polymec/")
include(polymec)
if (NOT DEFINED POLYMEC_VERSION)
  message(FATAL_ERROR "Could not find polymec library. Please use the polymec=/path/to/polymec option.")
endif()
message("-- Found polymec library in ${POLYMEC_PREFIX} (v${POLYMEC_VERSION})")

# Set us up to use polymec.
include_directories("${POLYMEC_PREFIX}/include")
link_directories("${POLYMEC_PREFIX}/lib")
link_libraries(${POLYMEC_LIBRARIES})

add_executable(meshy meshy.c)
