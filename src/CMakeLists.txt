# CMakeList.txt : CMake project for MapSimTextServ
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("MapSimTextServ")

# Add source to this project's executable.
add_executable (MapSimTextServ "MapSimTextServ.cpp" "MapSimTextServ.h")

# TODO: Add tests and install targets if needed.
target_include_directories(MapSimTextServ PUBLIC "include")

# Set compiler options for profiling
target_compile_options(MapSimTextServ PUBLIC "-pg")

# TODO: Add tests and install targets if needed.
target_include_directories(MapSimTextServ PUBLIC "include")