add_executable(test_small_sim test_small_simulation.cpp)
target_link_libraries(test_small_sim epideux_engine)
add_test(NAME small_simulation COMMAND test_small_sim)

add_executable(test_move_loc test_move_location.cpp)
target_link_libraries(test_move_loc epideux_engine)
add_test(NAME move_location COMMAND test_move_loc)
set_tests_properties(move_location PROPERTIES
  PASS_REGULAR_EXPRESSION "^Persons in location 1/2: 10/0 \\(2h\\) 0/10 \\(2h\\) 10/0"
)