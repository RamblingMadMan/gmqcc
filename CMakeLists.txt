cmake_minimum_required(VERSION 3.4)

project(gmqcc VERSION 0.0.1 LANGUAGES C CXX)

set(GMQCC_MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
	set(GMQCC_MASTER_PROJECT ON)
	message(STATUS "CMake version: ${CMAKE_VERSION}")
endif()

option(GMQCC_BUILD_TOOLS "Build executables" ${GMQCC_MASTER_PROJECT})
option(GMQCC_BUILD_TESTS "Build test cases" ${GMQCC_MASTER_PROJECT})

set(
	GMQCCLIB_SOURCE_FILES
	algo.h
	ast.cpp ast.h
	code.cpp
	conout.cpp
	fold.cpp fold.h
	ftepp.cpp
	gmqcc.h
	intrin.cpp intrin.h
	ir.cpp
	ir.h
	lexer.cpp lexer.h
	opts.cpp
	parser.cpp parser.h
	stat.cpp
	utf8.cpp
	util.cpp
)

add_library(gmqcclib STATIC ${GMQCCLIB_SOURCE_FILES})
set_target_properties(gmqcclib PROPERTIES CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

if(GMQCC_BUILD_TOOLS)
	add_executable(gmqcc main.cpp)
	set_target_properties(gmqcc PROPERTIES CXX_STANDARD 11)
	target_link_libraries(gmqcc gmqcclib)

	add_executable(qcvm exec.cpp)
	set_target_properties(qcvm PROPERTIES CXX_STANDARD 11)
	target_link_libraries(qcvm gmqcclib)
endif()

if(GMQCC_BUILD_TESTS)
	add_executable(testsuite test.cpp)
	set_target_properties(testsuite PROPERTIES CXX_STANDARD 11)
	target_link_libraries(testsuite gmqcclib)
endif()
