#include "raylib.h"
#include "MainScene.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{


    // Initialization
    //--------------------------------------------------------------------------------------
    constexpr int screenWidth = 1600;
    constexpr int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "raylib Marching Cubes");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    MainScene scene;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        scene.Run(deltaTime);
    }



    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}