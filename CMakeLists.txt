cmake_minimum_required(VERSION 3.12)
enable_testing()

project(pa3 CXX C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJ_LIBRARY ${PROJECT_NAME}common)
set(PROJ_PROGRAM ${PROJECT_NAME})
set(PROJ_TESTPROG ${PROJECT_NAME}_test)

# Normal source and header files
file(GLOB_RECURSE sources_common LIST_DIRECTORIES false CONFIGURE_DEPENDS src/common/*.cpp src/common/*.c)
file(GLOB_RECURSE sources_program LIST_DIRECTORIES false CONFIGURE_DEPENDS src/program/*.cpp src/program/*.c)
file(GLOB_RECURSE sources_test LIST_DIRECTORIES false CONFIGURE_DEPENDS src/test/*.cpp src/test/*.c)

set(include_common "include/common")
set(include_program "include/program")
set(include_test "include/test")

add_compile_options("-std=c++11")
add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-Wno-unused-parameter")

if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
	add_compile_options("-ggdb")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
	add_compile_options("-O2")
else()
	add_compile_options("-O0")
endif()

add_executable(${PROJ_PROGRAM} "${sources_program}")
add_executable(${PROJ_TESTPROG} "${sources_test}")
add_library(${PROJ_LIBRARY} "${sources_common}")

# Export symbols for dlsym
target_link_options(${PROJ_TESTPROG} PRIVATE "-Wl,--export-dynamic")

target_include_directories(${PROJ_LIBRARY} PUBLIC "${include_common}")
target_include_directories(${PROJ_PROGRAM} PRIVATE "${include_program}")
target_include_directories(${PROJ_TESTPROG} PRIVATE "${include_test}")

target_link_libraries(${PROJ_PROGRAM} ${PROJ_LIBRARY})
target_link_libraries(${PROJ_TESTPROG} ${PROJ_LIBRARY})

set(TEST_BINARY valgrind --error-exitcode=255 --leak-check=full $<TARGET_FILE:${PROJ_TESTPROG}>)
add_subdirectory(src/test)

