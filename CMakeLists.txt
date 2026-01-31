cmake_minimum_required(VERSION 3.15)
project(MarchingCubes)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.0
)

FetchContent_MakeAvailable(raylib)

add_executable(MarchingCubes src/main.cpp "src/MainScene.h" "src/MainScene.cpp" "src/Utils.h" "src/Utils.cpp" "src/DataTypes.h" "src/Tables.h" "src/Tables.cpp")
target_link_libraries(MarchingCubes raylib)