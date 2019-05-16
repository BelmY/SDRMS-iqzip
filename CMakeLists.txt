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
project(iqzip CXX C)
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
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Set the version information here
set(VERSION_INFO_MAJOR_VERSION 1)
set(VERSION_INFO_API_COMPAT    0)
set(VERSION_INFO_MINOR_VERSION 0)
set(VERSION_INFO_MAINT_VERSION git)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif()


########################################################################
# Add subdirectories
########################################################################
add_subdirectory(lib)
add_subdirectory(include/iqzip)
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
	
    ExternalProject_Add(aec_external
    	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libaec
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libaec
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libaec
        CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                   "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo Skipping installation of libaec
    )
    ExternalProject_Get_Property(aec_external binary_dir)
    add_library(aec SHARED IMPORTED GLOBAL)
    set_property(TARGET aec PROPERTY IMPORTED_LOCATION "${binary_dir}/src/libaec.so")
    
    add_dependencies(aec aec_external)
 
    set(AEC_LIBRARIES ${binary_dir}/src/libaec.so
	    	      ${binary_dir}/src/libaec.so.0
		      ${binary_dir}/src/libaec.so.0.0.10)

    set(AEC_INCLUDE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/libaec/src/libaec.h)

    install(FILES ${AEC_LIBRARIES} DESTINATION lib${LIB_SUFFIX})
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
find_package(SigMF REQUIRED)
if(NOT SIGMF_FOUND)
    message(WARNING "Libsigmf is not installed. The internal libsigmf will be automatically build and install.")
    include(ExternalProject)
	
    ExternalProject_Add(sigmf_external
    	GIT_REPOSITORY https://github.com/deepsig/libsigmf.git
    	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/libsigmf
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsigmf
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libsigmf
        CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
                   "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
        INSTALL_COMMAND ${CMAKE_COMMAND} -E echo Skipping installation of libsigmf
    )
    add_library(sigmf INTERFACE)
    ExternalProject_Get_Property(sigmf_external BINARY_DIR)
    ExternalProject_Get_Property(sigmf_external SOURCE_DIR)
 
	install(DIRECTORY ${SOURCE_DIR}/src/
		DESTINATION include/sigmf)
		
	install(DIRECTORY ${SOURCE_DIR}/sigmf_protocols/
		DESTINATION include/sigmf)
	
endif()

########################################################################
# Install cmake search helper for this library
########################################################################
if(NOT CMAKE_MODULES_DIR)
  set(CMAKE_MODULES_DIR lib${LIB_SUFFIX}/cmake)
endif(NOT CMAKE_MODULES_DIR)

install(FILES cmake/Modules/iqzipConfig.cmake
    DESTINATION ${CMAKE_MODULES_DIR}/iqzip
)

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

