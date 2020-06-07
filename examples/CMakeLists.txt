find_package(Python3 COMPONENTS Development NumPy)

add_executable(single_home single_home.cpp)
set_property(TARGET single_home PROPERTY CXX_STANDARD 17)
target_include_directories(single_home PRIVATE include ${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})
target_link_libraries(single_home epideux_engine Python3::Python Python3::NumPy)

add_executable(two_age two_age.cpp)
set_property(TARGET two_age PROPERTY CXX_STANDARD 17)
target_include_directories(two_age PRIVATE include ${Python3_INCLUDE_DIRS} ${Python3_NumPy_INCLUDE_DIRS})
target_link_libraries(two_age epideux_engine Python3::Python Python3::NumPy)