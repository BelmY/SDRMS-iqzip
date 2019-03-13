INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_AEC AEC)

FIND_PATH(
    AEC_INCLUDE_HEADERS
    NAMES libaec/libaec.h
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    AEC_LIBRARIES
    NAMES libaec.so
    PATHS /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AEC_FOUND DEFAULT_MSG AEC_LIBRARIES AEC_INCLUDE_HEADERS)
