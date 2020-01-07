INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SIGMF SigMF)

find_path(
    SIGMF_INCLUDE_DIR
    NAMES sigmf/sigmf.h
    PATHS /usr/local/include
          /usr/include
          /usr/local/opt/sigmf
)

set(SIGMF_INCLUDE_GEN_DIR ${SIGMF_INCLUDE_DIR}/sigmf)
set(SIGMF_FLATBUFFERS_INCLUDE_DIR ${SIGMF_INCLUDE_DIR}/sigmf/external)
set(SIGMF_JSON_INCLUDE_DIR ${SIGMF_INCLUDE_DIR}/sigmf/external)
set(SIGMF_INCLUDE_DIRS 
    ${SIGMF_INCLUDE_DIR} 
    ${SIGMF_FLATBUFFERS_INCLUDE_DIR} 
    ${SIGMF_JSON_INCLUDE_DIR} 
    ${SIGMF_INCLUDE_GEN_DIR})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SigMF DEFAULT_MSG SIGMF_INCLUDE_DIRS)