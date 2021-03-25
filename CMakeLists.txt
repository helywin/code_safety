cmake_minimum_required(VERSION 3.5)
project(code_security LANGUAGES CXX)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Debug编译")
    set(CMAKE_CXX_FLAGS "-pipe -pthread -g -ggdb -std=gnu++11 -Wall -D_REENTRANT -fPIC")
    set(CMAKE_C_FLAGS "-pipe -g -ggdb -Wall -D_REENTRANT -fPIC")
else () #CMAKE_BUILD_TYPE MATCHES Release
    message("Release编译")
    set(CMAKE_CXX_FLAGS "-pipe -O2 -std=gnu++11 -Wall -W -D_REENTRANT -fPIC")
    set(CMAKE_C_FLAGS "-pipe -O2 -Wall -W -D_REENTRANT -fPIC")
endif ()

include(FindPkgConfig)
pkg_check_modules(BREAKPAD_CLIENT REQUIRED breakpad-client)
message(STATUS "breakpad_client目录 ${BREAKPAD_CLIENT_INCLUDE_DIRS}")
include_directories(${BREAKPAD_CLIENT_INCLUDE_DIRS})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)

add_executable(test_breakpad test_breakpad.cpp func.cpp func.hpp)
target_link_libraries(test_breakpad
        ${BREAKPAD_CLIENT_LIBRARIES}
        pthread)

add_executable(test_coredump test_breakpad.cpp func.cpp func.hpp)
target_link_libraries(test_coredump pthread ${BREAKPAD_CLIENT_LIBRARIES})
target_compile_definitions(test_coredump PRIVATE -DWITHOUT_BREAKPAD)

add_executable(test_server test_breakpad_server.cpp func.cpp func.hpp)
target_link_libraries(test_server ${BREAKPAD_CLIENT_LIBRARIES})

add_executable(test_memory_asan test_memory.cpp)
set_target_properties(test_memory_asan PROPERTIES
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize-recover=address"
        LINK_FLAGS "-fsanitize=address -static-libasan"
        )

add_executable(test_memory test_memory.cpp)


#add_custom_command()