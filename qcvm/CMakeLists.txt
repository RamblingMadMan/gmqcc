add_executable(qcvm main.cpp)
set_target_properties(qcvm PROPERTIES CXX_STANDARD 11)
target_link_libraries(qcvm gmqcclib) 
