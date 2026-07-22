# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright (C) 2019 CAMELab
#
# Author: Donghyun Gouk <kukdh1@camelab.org>
#

# Set version and project name
cmake_minimum_required(VERSION 3.10)
project(mcpat)

# Validate build type
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Select CMake build type." FORCE)

  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif ()

string (TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE AND NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE)$")
  message (FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif ()

# Set output directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set include directories
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/cacti
)

# Specify source files
set(SRC_MCPAT_CACTI
  cacti/arbiter.cc
  cacti/area.cc
  cacti/bank.cc
  cacti/basic_circuit.cc
  cacti/cacti_interface.cc
  cacti/component.cc
  cacti/crossbar.cc
  cacti/decoder.cc
  cacti/htree2.cc
  cacti/io.cc
  cacti/main.cc
  cacti/mat.cc
  cacti/nuca.cc
  cacti/parameter.cc
  cacti/powergating.cc
  cacti/router.cc
  cacti/subarray.cc
  cacti/technology.cc
  cacti/uca.cc
  cacti/Ucache.cc
  cacti/wire.cc
)
set(SRC_MCPAT
  array.cc
  basic_components.cc
  core.cc
  interconnect.cc
  iocontrollers.cc
  logic.cc
  mcpat.cc
  memoryctrl.cc
  noc.cc
  processor.cc
  sharedcache.cc
  XML_Parse.cc
)

# Source group for MSVC
SOURCE_GROUP("Source Files\\" FILES ${SRC_MCPAT})
SOURCE_GROUP("Source Files\\cacti" FILES ${SRC_MCPAT_CACTI})

# Define library
add_library(mcpat STATIC
  ${SRC_MCPAT}
  ${SRC_MCPAT_CACTI}
)

# Platform specific settings
if (MSVC)
  target_compile_definitions(mcpat PRIVATE -D_CRT_SECURE_NO_WARNINGS)
  target_compile_options(mcpat PRIVATE /wd4819)  # Surpress unicode warning
  target_compile_options(mcpat PRIVATE /wd4305)  # Surpress truncation warning
  target_compile_options(mcpat PRIVATE /wd4800)  # Surpress int -> boolean waring
  target_compile_options(mcpat PRIVATE /wd4129)  # Surpress escape warning
  target_compile_options(mcpat PRIVATE /wd4244)  # Surpress conversion warning
  target_compile_options(mcpat PRIVATE /wd4267)  # Surpress conversion warning
else ()
  target_compile_options(mcpat PRIVATE -g)
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
