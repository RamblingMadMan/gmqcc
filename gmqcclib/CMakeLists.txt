set(
	GMQCCLIB_SOURCE_FILES
	ast.cpp
	code.cpp
	conout.cpp
	fold.cpp
	ftepp.cpp
	intrin.cpp
	ir.cpp
	lexer.cpp
	opts.cpp
	parser.cpp
	stat.cpp
	utf8.cpp
	util.cpp
)

add_library(gmqcclib STATIC ${GMQCCLIB_SOURCE_FILES})

target_include_directories(gmqcclib PUBLIC ${GMQCC_INCLUDE_DIR})

set_target_properties(
	gmqcclib PROPERTIES
	CXX_STANDARD 11
	CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti"
)
