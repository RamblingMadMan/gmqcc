add_executable(gmqcc main.cpp)
set_target_properties(gmqcc PROPERTIES CXX_STANDARD 11)
target_link_libraries(gmqcc gmqcclib) 
