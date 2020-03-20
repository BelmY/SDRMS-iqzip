# IQzip
#
# Copyright (C) 2019, Libre Space Foundation <https://libre.space/>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
#

# ##############################################################################
# Project setup
# ##############################################################################
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(iqzip CXX C)
enable_testing()

# Enable C++14 support
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")

# select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)

# make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Set the version information here
set(VERSION_MAJOR 0)
set(VERSION_API 0)
set(VERSION_ABI 10)
set(VERSION_PATCH git)

if(VERSION_PATCH STREQUAL "git")
    execute_process(COMMAND git log -1 --format=%h
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_COMMIT_HASH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(VERSION_PATCH "${GIT_COMMIT_HASH}")
endif()
set(LIBVERSION "${VERSION_MAJOR}.${VERSION_API}.${VERSION_ABI}-${VERSION_PATCH}")

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

# ##############################################################################
# Install cmake helper modules
# ##############################################################################
if(UNIX)
  set(CMAKE_LIB_DEST share/cmake/${PROJECT_NAME})
elseif(WIN32)
  set(CMAKE_LIB_DEST cmake)
endif()

install(FILES ${PROJECT_SOURCE_DIR}/cmake/Modules/iqzipConfig.cmake
        DESTINATION ${CMAKE_LIB_DEST})

# ##############################################################################
# Install directories
# ##############################################################################
set(IQZIP_RUNTIME_DIR bin)
set(IQZIP_LIBRARY_DIR lib${LIB_SUFFIX})
set(IQZIP_DATA_DIR share)
set(IQZIP_DOC_DIR ${IQZIP_DATA_DIR}/doc)
set(IQZIP_PKG_DOC_DIR ${IQZIP_DOC_DIR}/${CMAKE_PROJECT_NAME})
set(IQZIP_CONF_DIR etc)
set(IQZIP_PKG_CONF_DIR ${IQZIP_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d)
set(IQZIP_LIBEXEC_DIR libexec)
set(IQZIP_PKG_LIBEXEC_DIR ${IQZIP_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})

# ##############################################################################
# Search for the dependencies
# ##############################################################################
find_package(AEC REQUIRED)

# ##############################################################################
# Setup doxygen option
# ##############################################################################
find_package(Doxygen)
if(DOXYGEN_FOUND)
  option(ENABLE_DOXYGEN "Build docs using Doxygen" ON)
  add_subdirectory(docs)
else(DOXYGEN_FOUND)
  option(ENABLE_DOXYGEN "Build docs using Doxygen" OFF)
endif(DOXYGEN_FOUND)


# ##############################################################################
# Create uninstall target
# ##############################################################################
configure_file(${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake @ONLY)

add_custom_target(uninstall
                  ${CMAKE_COMMAND}
                  -P
                  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/lib
    ${CMAKE_BINARY_DIR}/include
)

# ##############################################################################
# Add subdirectories
# ##############################################################################
add_subdirectory(apps)
add_subdirectory(include/iqzip)
add_subdirectory(lib)




