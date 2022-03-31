cmake_minimum_required(VERSION 3.18)
project(libtellurium)

set(SOURCES src/core.c
    src/format.c
    src/logger/console.c
    src/logger/file.c
    src/logger/logger.c
    src/logger/syslog.c)



add_library(tellurium SHARED ${SOURCES})
set_target_properties(tellurium PROPERTIES SOVERSION 0)
set_target_properties(tellurium PROPERTIES PUBLIC_HEADER include/tellurium.h)

target_include_directories(tellurium PUBLIC include)
target_include_directories(tellurium PRIVATE src)



