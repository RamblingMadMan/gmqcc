cmake_minimum_required(VERSION 3.4)

project(gmqcc VERSION 0.0.1 LANGUAGES C CXX)

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

add_executable(gmqcc main.cpp)
set_target_properties(gmqcc PROPERTIES CXX_STANDARD 11)
target_link_libraries(gmqcc gmqcclib)

add_executable(testsuite test.cpp)
set_target_properties(testsuite PROPERTIES CXX_STANDARD 11)
target_link_libraries(testsuite gmqcclib)

add_executable(qcvm exec.cpp)
set_target_properties(qcvm PROPERTIES CXX_STANDARD 11)
target_link_libraries(qcvm gmqcclib)
