/*******************************************************************************************
 *
 *   raylib [shapes] example - rlgl color wheel
 *
 *   Example complexity rating: [★★★☆] 3/4
 *
 *   Example originally created with raylib 6.0, last time updated with raylib 6.0
 *
 *   Example contributed by Robin (@RobinsAviary) and reviewed by Ramon Santamaria (@raysan5)
 *
 *   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
 *   BSD-like license that allows static linking with closed source software
 *
 *   Copyright (c) 2025 Robin (@RobinsAviary)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    // The minimum/maximum points the circle can have
    const unsigned int pointsMin = 3;
    const unsigned int pointsMax = 256;

    // The current number of points and the radius of the circle
    unsigned int triangleCount = 64;
    float pointScale = 150.0f;

    // Slider value, literally maps to value in HSV
    float value = 1.0f;

    // The center of the screen
    Vector2 center = {(float)screenWidth / 2.0f, (float)screenHeight / 2.0f};
    // The location of the color wheel
    Vector2 circlePosition = center;

    // The currently selected color
    Color color = {255, 255, 255, 255};

    // Indicates if the slider is being clicked
    bool sliderClicked = false;

    // Indicates if the current color going to be updated, as well as the handle position
    bool settingColor = false;

    // How the color wheel will be rendered
    unsigned int renderType = RL_TRIANGLES;

    // Enable anti-aliasing
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - rlgl color wheel");

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        Rectangle sliderRectangle = {42.0f, 16.0f + 64.0f + 45.0f, 64.0f, 16.0f};
        Vector2 mousePosition = GetMousePosition();

        // Checks if the user is hovering over the value slider
        bool sliderHover = (mousePosition.x >= sliderRectangle.x && mousePosition.y >= sliderRectangle.y && mousePosition.x < sliderRectangle.x + sliderRectangle.width && mousePosition.y < sliderRectangle.y + sliderRectangle.height);

        // Checks if the user clicked on the color wheel
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Vector2Distance(GetMousePosition(), center) <= pointScale + 10.0f)
        {
            settingColor = true;
        }

        // Update flag when mouse button is released
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            settingColor = false;

        // Check if the user clicked/released the slider for the color's value
        if (sliderHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            sliderClicked = true;

        if (sliderClicked && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            sliderClicked = false;

        // Update render mode accordingly
        if (IsKeyPressed(KEY_SPACE))
            renderType = RL_LINES;

        if (IsKeyReleased(KEY_SPACE))
            renderType = RL_TRIANGLES;

        // If the slider or the wheel was clicked, update the current color
        if (settingColor || sliderClicked)
        {
            if (settingColor)
                circlePosition = GetMousePosition();

            float distance = Vector2Distance(center, circlePosition) / pointScale;

            float angle = ((Vector2Angle((Vector2){0.0f, -pointScale}, Vector2Subtract(center, circlePosition)) / PI + 1.0f) / 2.0f);
            if (settingColor && distance > 1.0f)
                circlePosition = Vector2Add((Vector2){sinf(angle * (PI * 2.0f)) * pointScale, -cosf(angle * (PI * 2.0f)) * pointScale}, center);

            float angle360 = angle * 360.0f;
            float valueActual = Clamp(distance, 0.0f, 1.0f);
            color = ColorLerp((Color){(int)(value * 255.0f), (int)(value * 255.0f), (int)(value * 255.0f), 255}, ColorFromHSV(angle360, Clamp(distance, 0.0f, 1.0f), 1.0f), valueActual);
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Begin rendering color wheel
        

        // Make the handle slightly more visible overtop darker colors
        Color handleColor = BLACK;

        if (Vector2Distance(center, circlePosition) / pointScale <= 0.5f && value <= 0.5f)
        {
            handleColor = DARKGRAY;
        }

        // Draw the color handle
        DrawCircleLinesV(circlePosition, 4.0f, handleColor);

        // Draw the color in a preview, with a darkened outline.
        DrawRectangleV((Vector2){8.0f, 8.0f}, (Vector2){64.0f, 64.0f}, color);
        DrawRectangleLinesEx((Rectangle){8.0f, 8.0f, 64.0f, 64.0f}, 2.0f, ColorLerp(color, BLACK, 0.5f));

        // Draw current color as hex and decimal
        DrawText(TextFormat("#%02X%02X%02X\n(%d, %d, %d)", color.r, color.g, color.b, color.r, color.g, color.b), 8, 8 + 64 + 8, 20, DARKGRAY);

        // Update the visuals for the copying text
        Color copyColor = DARKGRAY;
        unsigned int offset = 0;

        EndDrawing();
    }

    CloseWindow();

    return 0;
}