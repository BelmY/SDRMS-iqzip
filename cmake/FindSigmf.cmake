set(SIGMF_FOUND TRUE)
set(SIGMF_ROOT @CMAKE_INSTALL_PREFIX@)
find_path(
    SIGMF_ROOT
    NAMES include/sigmf/sigmf.h
    PATHS /usr/local/include
          /usr/include
          /usr/local/opt/sigmf/
          $ENV{SIGMF_ROOT}
          ${SIGMF_ROOT}
)
set(SIGMF_INCLUDE_DIR ${SIGMF_ROOT}/include)
set(SIGMF_INCLUDE_GEN_DIR ${SIGMF_ROOT}/include/sigmf/)
set(SIGMF_FLATBUFFERS_INCLUDE_DIR ${SIGMF_ROOT}/include/sigmf/external/)
set(SIGMF_JSON_INCLUDE_DIR ${SIGMF_ROOT}/include/sigmf/external/)
set(SIGMF_INCLUDE_DIRS ${SIGMF_INCLUDE_DIR} ${SIGMF_FLATBUFFERS_INCLUDE_DIR} ${SIGMF_JSON_INCLUDE_DIR} ${SIGMF_INCLUDE_GEN_DIR})
message(STATUS "Found Sigmf root: " ${SIGMF_ROOT})
