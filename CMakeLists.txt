# IQzip
#
# Copyright (C) 2019, Libre Space Foundation <https://libre.space/>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(Iqzip CXX C)
enable_testing()

# Enable C++14 support
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-std=c++14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")

#select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
#IGNORE    set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
#IGNORE set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

#make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

# Set the version information here
set(VERSION_INFO_MAJOR_VERSION 1)
set(VERSION_INFO_API_COMPAT    0)
set(VERSION_INFO_MINOR_VERSION 0)
set(VERSION_INFO_MAINT_VERSION git)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()

########################################################################
# Install cmake helper modules
########################################################################
if (UNIX)
	set(CMAKE_LIB_DEST share/cmake/${PROJECT_NAME})
elseif (WIN32)
	set(CMAKE_LIB_DEST cmake)
endif()

install(FILES
	${PROJECT_SOURCE_DIR}/cmake/IqzipConfig.cmake
	DESTINATION ${CMAKE_LIB_DEST})

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(lib)
add_subdirectory(apps)

########################################################################
# Compiler specific setup
########################################################################
if(CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    add_definitions(-fvisibility=hidden)
endif()

########################################################################
# Install directories
########################################################################
set(IQZIP_RUNTIME_DIR      bin)
set(IQZIP_LIBRARY_DIR      lib${LIB_SUFFIX})
set(IQZIP_DATA_DIR         share)
set(IQZIP_DOC_DIR          ${IQZIP_DATA_DIR}/doc)
set(IQZIP_PKG_DOC_DIR      ${IQZIP_DOC_DIR}/${CMAKE_PROJECT_NAME})
set(IQZIP_CONF_DIR         etc)
set(IQZIP_PKG_CONF_DIR     ${IQZIP_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d)
set(IQZIP_LIBEXEC_DIR      libexec)
set(IQZIP_PKG_LIBEXEC_DIR  ${IQZIP_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})

#########################################################################
# Search for the libaec if it is already installed in the system
# If not, install the internal one.
#########################################################################
find_package(AEC REQUIRED)
if(NOT AEC_FOUND)
    message(WARNING "libaec is not installed. The internal libaec will be automatically build and install.")
    include(ExternalProject)
	
    ExternalProject_Add(AEC_EXTERNAL
    	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libaec
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libaec
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libaec
        CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                   "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo Skipping installation of libaec
    )
    ExternalProject_Get_Property(AEC_EXTERNAL binary_dir)
    add_library(aec SHARED IMPORTED GLOBAL)
    set_property(TARGET aec PROPERTY IMPORTED_LOCATION "${binary_dir}/src/libaec.so")
    
    add_dependencies(aec AEC_EXTERNAL)
 
    set(AEC_LIBRARIES ${binary_dir}/src/libaec.so
	    	      ${binary_dir}/src/libaec.so.0
		      ${binary_dir}/src/libaec.so.0.0.10)

    set(AEC_INCLUDE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/libaec/src/libaec.h)

    install(FILES ${AEC_LIBRARIES} DESTINATION lib${LIB_SUFFIX})
		    message(${binary_dir})
    install(FILES ${AEC_INCLUDE_HEADERS}	      
 	    DESTINATION include)

else()
    add_library(aec INTERFACE)
endif()

########################################################################
# Setup doxygen option
########################################################################
find_package(Doxygen)
if(DOXYGEN_FOUND)
	option(ENABLE_DOXYGEN "Build docs using Doxygen" ON)
	add_subdirectory(docs)
else(DOXYGEN_FOUND)
	option(ENABLE_DOXYGEN "Build docs using Doxygen" OFF)
endif(DOXYGEN_FOUND)

########################################################################
# Search for libsigmf
########################################################################
set(LIBSIGMF_INCLUDE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/libsigmf/src
	CACHE PATH "libsigmf include directory")

if (NOT IS_DIRECTORY ${LIBSIGMF_INCLUDE_DIR})
    message("Libsigmf not found! Downloading...")
    execute_process(COMMAND git submodule update --init -- libsigmf
	    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif(NOT IS_DIRECTORY ${LIBSIGMF_INCLUDE_DIR})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libsigmf 
	${CMAKE_BINARY_DIR}/sigmf-build
	EXCLUDE_FROM_ALL)

file(GLOB SIGMF_HEADER ${LIBSIGMF_INCLUDE_DIR}/*.h)
set(SIGMF_GEN_HEADERS_DIR ${CMAKE_BINARY_DIR}/sigmf-build/include) 
file(GLOB SIGMF_GEN_HEADERS ${SIGMF_GEN_HEADERS_DIR}/*.h)

set(JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsigmf/external/json/include)

#########################################################################
## Check updates on libsigmf
#########################################################################
##TODO: Automatically check libsigmf repo for changes. If exist, rebuild.
#find_package(Git REQUIRED)
#if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
## Update submodules as needed
#    option(GIT_SUBMODULE "Check submodules during build" ON)
#    if(GIT_SUBMODULE)
#        message(STATUS "Submodule libsigmf update")
#        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
#                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
#        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
#            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
#        endif()
#    endif()
#endif()
#
#if(NOT EXISTS "${PROJECT_SOURCE_DIR}/libsigmf/CMakeLists.txt")
#    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
#endif()

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Install cmake search helper for this library
########################################################################
if(NOT CMAKE_MODULES_DIR)
  set(CMAKE_MODULES_DIR lib${LIB_SUFFIX}/cmake)
endif(NOT CMAKE_MODULES_DIR)

#install(FILES cmake/iqzipConfig.cmake
#    DESTINATION ${CMAKE_MODULES_DIR}/iqzip
#)

########################################################################
# Install Iqzip headers
########################################################################
install(DIRECTORY include/iqzip DESTINATION include)

########################################################################
# Install sigmf headers
########################################################################
install(FILES ${SIGMF_HEADER}
	DESTINATION include)
install(FILES ${SIGMF_GEN_HEADERS} 
	DESTINATION include)

########################################################################
# Install flatbuffers headers
########################################################################
install(DIRECTORY ${FLATBUFFERS_INCLUDE_DIR}/flatbuffers
	DESTINATION include
	FILES_MATCHING PATTERN "*.h")

########################################################################
# Install json headers
########################################################################
install(DIRECTORY ${JSON_INCLUDE_DIR}/nlohmann
	DESTINATION include
	FILES_MATCHING PATTERN "*.hpp*")
