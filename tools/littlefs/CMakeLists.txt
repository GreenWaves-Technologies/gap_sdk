cmake_minimum_required(VERSION 3.5.1)
project(LittleFS-tools C)

set(CMAKE_C_STANDARD 11)

add_executable(mklfs
        lfs/lfs.c
        lfs/lfs.h
        lfs/lfs_util.c
        lfs/lfs_util.h
        mklfs.c)



add_executable(lslfs
        lfs/lfs.c
        lfs/lfs.h
        lfs/lfs_util.c
        lfs/lfs_util.h
        lslfs.c)

install(TARGETS mklfs lslfs DESTINATION bin)
