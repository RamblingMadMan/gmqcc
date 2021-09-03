add_executable(testsuite main.cpp)
set_target_properties(testsuite PROPERTIES CXX_STANDARD 11)
target_link_libraries(testsuite gmqcclib) 
