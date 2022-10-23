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

find_package(safestringlib COMPONENTS system filesystem)
if(NOT safestringlib_FOUND)
    add_subdirectory(contrib/safestringlib)
endif()
add_subdirectory(contrib/libtermcolor)


target_link_libraries(tellurium PRIVATE safestring_shared)
target_link_libraries(tellurium PRIVATE termcolors)



# very basic testing (for now, later something else)

add_executable(telluriumtest test/basic.c)
target_link_libraries(telluriumtest tellurium)


