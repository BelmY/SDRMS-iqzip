INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_LIBTAR LIBTAR)

FIND_PATH(
    LIBTAR_INCLUDE_HEADERS
    NAMES libtar.h libtar_listhash.h
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    LIBTAR_LIBRARIES
    NAMES libtar.so
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBTAR DEFAULT_MSG LIBTAR_LIBRARIES LIBTAR_INCLUDE_HEADERS)
