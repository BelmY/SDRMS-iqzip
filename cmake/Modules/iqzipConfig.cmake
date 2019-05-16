INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_IQZIP iqzip)

if(DEFINED INCLUDED_IQZIP_CONFIG_CMAKE)
    return()
endif()
set(INCLUDED_IQZIP_CONFIG_CMAKE TRUE)

########################################################################
# Automatic LIB_SUFFIX detection + configuration option
########################################################################
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif()

if(LINUX AND EXISTS "/etc/debian_version")
    set(DEBIAN TRUE)
endif()

if(LINUX AND EXISTS "/etc/redhat-release")
    set(REDHAT TRUE)
endif()

if(LINUX AND EXISTS "/etc/SuSE-release")
    set(SUSE TRUE)
endif()

if(LINUX AND EXISTS "/etc/slackware-version")
    set(SLACKWARE TRUE)
endif()

if(NOT DEFINED LIB_SUFFIX AND (REDHAT OR SUSE OR SLACKWARE) AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
    SET(LIB_SUFFIX 64)
endif()
set(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "lib directory suffix")

########################################################################
# Search for iqzip
########################################################################

FIND_PATH(
    IQZIP_INCLUDE_DIRS
    NAMES iqzip/iqzip.h
    HINTS $ENV{IQZIP_DIR}/include
        ${PC_IQZIP_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    IQZIP_LIBRARIES
    NAMES iqzip
    HINTS $ENV{IQZIP_DIR}/lib
        ${PC_IQZIP_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(IQZIP DEFAULT_MSG IQZIP_LIBRARIES IQZIP_INCLUDE_DIRS)
MARK_AS_ADVANCED(IQZIP_LIBRARIES IQZIP_INCLUDE_DIRS)

