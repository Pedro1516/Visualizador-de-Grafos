#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

typedef struct circle
{
    int x;
    int y;
    int radius;
    Color cor;
} Circle;

typedef struct aresta
{
    int startX;
    int startY;
    int endX;
    int endY;
    Color cor;
    int vertice[2];
} Aresta;

typedef struct vertice
{
    Circle v;
    Aresta *arestas;
} Vertice;

int main()
{
    Vertice *vertices;
    Aresta *arestas;

    Camera2D camera = {
        (Vector2){0, 0},
        (Vector2){0, 0},
        0,
        1,
    };

    InitWindow(600, 800, "Grafo");
    SetTargetFPS(20);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);
        EndMode2D();
        EndDrawing();
    };

    CloseWindow();
    return 0;
}