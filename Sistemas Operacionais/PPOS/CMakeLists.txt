cmake_minimum_required(VERSION 3.20)
project(PPOS C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Tests:
#P1: test/testafila.c source/queue.c
#P2: test/pingpong-tasks1.c source/ppos-core.c source/queue.c | test/pingpong-tasks2.c source/ppos-core.c source/queue.c | test/pingpong-tasks3.c source/ppos-core.c source/queue.c
#P3: test/pingpong-dispatcher.c source/ppos-core.c source/queue.c
#P4: test/pingpong-scheduler.c source/ppos-core.c source/queue.c
#P5: test/pingpong-preempcao.c source/ppos-core.c source/queue.c | test/pingpong-preempcao-stress.c source/ppos-core.c source/queue.c
#P6: test/pingpong-contab.c source/ppos-core.c source/queue.c | test/pingpong-contab-prio.c source/ppos-core.c source/queue.c
#P7: test/pingpong-maintask.c source/ppos-core.c source/queue.c
#A8: test/nada.c source/queue.c | test/alternancia.c source/queue.c | test/peterson.c source/queue.c | test/temporizado.c source/queue.c
#A9: test/semaforo.c source/queue.c | prodcons.c source/queue.c | test/leitor_escritor.c source/queue.c
#P8: test/pingpong-join.c source/ppos-core.c source/queue.c
#P9: test/pingpong-sleep.c source/ppos-core.c source/queue.c

set(SOURCE_FILES test/pingpong-sleep.c source/ppos-core.c source/queue.c)

add_executable(PPOS ${SOURCE_FILES})

include(FindPkgConfig)
find_package(PPOS REQUIRED)
include_directories(${PPOS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${PPOS_LIBRARY})