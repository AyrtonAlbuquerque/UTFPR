set(FIND_PPOS_PATHS
        ${PROJECT_SOURCE_DIR}/library)

find_path(PPOS_INCLUDE_DIR
        disk.h ppos.h
        ppos-core-globals.h
        ppos_data.h
        ppos_disk.h
        queue.h
        PATH_SUFFIXES include
        PATHS ${FIND_PPOS_PATHS})

find_library(PPOS_LIBRARY
        NAMES ppos_static
        PATH_SUFFIXES lib
        PATHS ${FIND_PPOS_PATHS})