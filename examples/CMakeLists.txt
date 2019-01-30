
add_executable(generic_connect generic_connect.cpp)
add_executable(generic_dmm generic_dmm.cpp)
add_executable(m2k_connect m2k_connect.cpp)
add_executable(m2k_ps m2k_ps.cpp)
add_executable(connect connect.cpp)

target_link_libraries(generic_connect m2k)
target_link_libraries(m2k_connect m2k)
target_link_libraries(generic_dmm m2k)
target_link_libraries(m2k_ps m2k)
target_link_libraries(connect m2k)
