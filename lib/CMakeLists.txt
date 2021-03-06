# Copyright 2019 Free Software Foundation, Inc.
#
# This file was generated by gr_modtool, a tool from the GNU Radio framework
# This file is a part of iqzip
#
# GNU Radio is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3, or (at your option) any later version.
#
# GNU Radio is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# GNU Radio; see the file COPYING.  If not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Boston, MA 02110-1301, USA.

# ##############################################################################
# Setup library
# ##############################################################################

list(APPEND iqzip_sources
            ccsds_packet_primary_header.cpp
            compression_identification_packet.cpp
            iqzip_compression_header.cpp
            )

list(APPEND IQZIP_INCLUDE_DIRS
            ${PROJECT_SOURCE_DIR}/include
            ${AEC_INCLUDE_DIR})

if(NOT iqzip_sources)
  message(STATUS "No C++ sources... skipping lib/")
  return()
endif(NOT iqzip_sources)

if(APPLE)
  set_target_properties(iqzip
                        PROPERTIES INSTALL_NAME_DIR
                                   "${CMAKE_INSTALL_PREFIX}/lib")
endif(APPLE)

add_library(iqzip SHARED ${iqzip_sources})
target_link_libraries(iqzip ${AEC_LIBRARIES})

target_sources(iqzip PRIVATE
    iqzip_impl.cpp
    compressor_impl.cpp
    decompressor_impl.cpp
    )

target_include_directories(iqzip
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  )
  
set_target_properties(iqzip
                      PROPERTIES VERSION
                                 ${LIBVERSION}
                                 SOVERSION
                                 ${VERSION_MAJOR})
set_target_properties(iqzip PROPERTIES DEFINE_SYMBOL "iqzip_EXPORTS")

# ##############################################################################
# Install lib
# ##############################################################################
install(TARGETS iqzip LIBRARY DESTINATION lib${LIB_SUFFIX})

# ##############################################################################
# Print summary
# ##############################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${LIBVERSION} / ${VERSION_MAJOR}")
