cmake_minimum_required(VERSION 3.17)

project(epideux)

include_directories(include)

add_library(epideux_engine STATIC src/model.cpp
src/person.cpp
src/location.cpp
src/itinerary_entry.cpp
include/epideux/epideux.h)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
include(CTest)
add_subdirectory(tests)
endif()

add_subdirectory(experiments)
