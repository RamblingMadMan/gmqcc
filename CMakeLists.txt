cmake_minimum_required(VERSION 3.4)

project(gmqcc VERSION 0.0.1 LANGUAGES C CXX)

set(GMQCC_MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(GMQCC_MASTER_PROJECT ON)
	message(STATUS "CMake version: ${CMAKE_VERSION}")
endif()

option(GMQCC_BUILD_TOOLS "Build executables" ${GMQCC_MASTER_PROJECT})
option(GMQCC_BUILD_TESTS "Build test cases" ${GMQCC_MASTER_PROJECT})

set(GMQCC_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(gmqcclib)

if(GMQCC_BUILD_TOOLS)
	add_subdirectory(gmqcc)
	add_subdirectory(qcvm)
endif()

if(GMQCC_BUILD_TESTS)
	add_subdirectory(testsuite)
endif()
