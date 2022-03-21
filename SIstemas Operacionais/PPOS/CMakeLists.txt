cmake_minimum_required(VERSION 3.20)
project(PPOS C)

set(CMAKE_C_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#test/main.c
#source/disk.c
#source/queue.c
#source/timer.c
#source/signal.c
#source/ppos-core-aux.c
#source/pingpong-sleep.c
#source/pingpong-semaphore.c
#source/pingpong-scheduler.c
#source/pingpong-racecond.c
#source/pingpong-prodcons.c
#source/pingpong-preempcao-stress.c
#source/pingpong-preempcao.c
#source/pingpong-mqueue.c
#source/pingpong-join.c
#source/pingpong-disco2.c
#source/pingpong-disco1.c
#source/pingpong-contab-prio.c

# Tests:
#p1: test/main.c source/queue.c
#p2: test/pingpong-tasks1.c test/pingpong-tasks2.c test/pingpong-tasks3.c source/ppos-core.c

set(SOURCE_FILES test/main.c source/queue.c source/ppos-core.c)

add_executable(PPOS ${SOURCE_FILES})

include(FindPkgConfig)
find_package(PPOS REQUIRED)
include_directories(${PPOS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${PPOS_LIBRARY})