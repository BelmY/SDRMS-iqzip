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
# In-tree settings
########################################################################
if (IQZIP_IN_TREE_SOURCE_DIR)
    set(IQZIP_ROOT ${CMAKE_INSTALL_PREFIX})
    set(Iqzip_INCLUDE_DIRS ${IQZIP_IN_TREE_SOURCE_DIR}/include)
    set(Iqzip_LIBRARIES Iqzip)
    return()
endif (IQZIP_IN_TREE_SOURCE_DIR)
