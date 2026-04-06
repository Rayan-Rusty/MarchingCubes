cmake_minimum_required(VERSION 3.15)
project(MarchingCubes)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
set(GRAPHICS GRAPHICS_API_OPENGL_43)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git

)

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
        fastnoiselite
        GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
        GIT_TAG master
)
FetchContent_MakeAvailable(fastnoiselite)

add_executable(MarchingCubes src/main.cpp "src/MainScene.h" "src/MainScene.cpp" "src/Utils.h" "src/Utils.cpp" "src/DataTypes.h" "src/Tables.h" "src/Tables.cpp")


add_custom_command(TARGET MarchingCubes POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:MarchingCubes>/Resources
)


target_link_libraries(MarchingCubes raylib)

target_include_directories(MarchingCubes PRIVATE ${fastnoiselite_SOURCE_DIR}/Cpp)

