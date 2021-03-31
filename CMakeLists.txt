cmake_minimum_required(VERSION 3.5)
project(code_security LANGUAGES CXX)
enable_testing()

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message("Debug编译")
    set(CMAKE_CXX_FLAGS "-pipe -O0 -g -ggdb -std=gnu++11 -Wall -D_REENTRANT -fPIC")
    set(CMAKE_C_FLAGS "-pipe -O0 -g -ggdb -Wall -D_REENTRANT -fPIC")
else () #CMAKE_BUILD_TYPE MATCHES Release
    message("Release编译")
    set(CMAKE_CXX_FLAGS "-pipe -O2 -std=gnu++11 -Wall -W -D_REENTRANT -fPIC")
    set(CMAKE_C_FLAGS "-pipe -O2 -Wall -W -D_REENTRANT -fPIC")
endif ()

include(FindPkgConfig)
pkg_check_modules(BREAKPAD_CLIENT REQUIRED breakpad-client)
message(STATUS "breakpad_client目录 ${BREAKPAD_CLIENT_INCLUDE_DIRS}")
include_directories(${BREAKPAD_CLIENT_INCLUDE_DIRS})
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output)

# 查找clang-tidy
find_program(CLANG_TIDY_EXE NAMES "clang-tidy-12" DOC "Path to clang-tidy executable")
if (NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found.")
else ()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,-clang-analyzer-alpha.*")
endif ()

add_executable(test_breakpad test_breakpad.cpp func.cpp func.hpp)
target_link_libraries(test_breakpad
        ${BREAKPAD_CLIENT_LIBRARIES}
        pthread)

add_executable(test_server test_breakpad_server.cpp func.cpp func.hpp)
target_link_libraries(test_server ${BREAKPAD_CLIENT_LIBRARIES} pthread)
add_executable(test_memory_asan test_memory.cpp)
set_target_properties(test_memory_asan PROPERTIES
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize-recover=address"
        LINK_FLAGS "-fsanitize=address -static-libasan"
        )

add_executable(test_memory_none test_memory.cpp)

add_executable(test_clang_tidy test_breakpad.cpp func.cpp func.hpp)
target_link_libraries(test_clang_tidy
        ${BREAKPAD_CLIENT_LIBRARIES}
        pthread)
if (CLANG_TIDY_EXE)
    set_target_properties(test_clang_tidy PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif ()

add_executable(test_gcov test_gcov.cpp)
set_target_properties(test_gcov PROPERTIES
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage"
        )
target_link_libraries(test_gcov gcov)

get_target_property(dir test_gcov BINARY_DIR)
message(build at ${dir})

add_custom_target(run_test_gcov COMMAND
        ${CMAKE_SOURCE_DIR}/make_cov.sh ${dir}/CMakeFiles/test_gcov.dir
        )

add_executable(test_race_none test_race.cpp)
target_link_libraries(test_race_none pthread)

add_custom_target(test_race COMMAND
        clang-12 -lstdc++ -pthread -fsanitize=thread -g -O0 ${CMAKE_SOURCE_DIR}/test_race.cpp -o ${CMAKE_BINARY_DIR}/test_race
        )

add_custom_target(test_memory COMMAND
        clang-12 -lstdc++ -fsanitize=memory -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor -g -O0 ${CMAKE_SOURCE_DIR}/test_memory.cpp -o ${CMAKE_BINARY_DIR}/test_memory
        )

add_executable(test_gperf test_gperf.cpp)
target_link_libraries(test_gperf tcmalloc profiler)

#add_custom_command()