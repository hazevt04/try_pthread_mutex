cmake_minimum_required( VERSION 2.8.9 )

project( try_pthread_mutex )

include_directories( $CMAKE_SOURCE_DIR )

set(SRCS
   "main.c"
)

option(DEBUG_MACRO "Debug verbosity option" OFF)

if(DEBUG_MACRO)
   add_definitions(-DDEBUG)
endif()

add_executable( try_pthread_mutex "${SRCS}" )

target_link_libraries( try_pthread_mutex pthread )

