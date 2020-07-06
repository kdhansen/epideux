cmake_minimum_required(VERSION 3.16)

project(epideux)

include_directories(include)

add_library(epideux_engine STATIC src/model.cpp
src/person.cpp
src/location.cpp
src/itinerary_entry.cpp
include/epideux/epideux.h)

include(CTest)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()

option(BUILD_EXAMPLES "Build the C++ examples" ON)
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
