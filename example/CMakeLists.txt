cmake_minimum_required (VERSION 3.15)

project("highs-example" CXX)
find_package(highs REQUIRED) 

# Any source files added to include/ or source/ will automatically be added to the project.
file(GLOB_RECURSE INCLUDE_FILES ../src/*.h)
file(GLOB_RECURSE SOURCE_FILES ../src/*.cpp)

add_executable(${PROJECT_NAME} ${INCLUDE_FILES} ${SOURCE_FILES} main.cpp)
target_link_libraries(${PROJECT_NAME} highs::highs)
target_include_directories(${PROJECT_NAME} PUBLIC ../src)
