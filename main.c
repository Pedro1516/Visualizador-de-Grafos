#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

typedef struct circle
{
    int x;
    int y;
    float radius;
    Color cor;
} Circle;

typedef struct aresta
{
    Vector2 startpos;
    Vector2 endpos;
    int peso;
    int vertice[2];
    Color cor;
} Aresta;

typedef struct vertice
{
    Circle v;
    Aresta **arestas;
    int quant_a;
} Vertice;

typedef struct grafo
{
    Aresta *arestas;
    Vertice *vertices;
    int quant_v;
    int quant_a;
    int limit[2]; // memoria alocada para v e a. indice 0 para vertice e 1 para aresta
} Grafo;

void add_vertice(Grafo *grafo, int posx, int posy, Color color)
{
    if (grafo->quant_v + 1 > grafo->limit[0])
    {
        grafo->vertices = (Vertice *)realloc(grafo->vertices, sizeof(Vertice) * (grafo->limit[0] + 10));
        grafo->limit[0] += 10;
    }

    grafo->quant_v++;
    grafo->vertices[grafo->quant_v - 1].v = (Circle){posx, posy, 20, color};
    grafo->vertices[grafo->quant_v - 1].arestas = (Aresta **)malloc(sizeof(Aresta *) * grafo->limit[1] / 5);
}

void add_aresta(Grafo *grafo, int v1, int v2, int peso)
{
    if (grafo->quant_a + 1 > grafo->limit[1])
    {
        grafo->arestas = (Aresta *)realloc(grafo->arestas, sizeof(Aresta) * (grafo->limit[1] + 10));
        grafo->limit[1] += 10;
    }

    grafo->quant_a++;
    grafo->arestas[grafo->quant_a - 1] = (Aresta){(Vector2){grafo->vertices[v1].v.x, grafo->vertices[v1].v.y}, (Vector2){grafo->vertices[v2].v.x, grafo->vertices[v2].v.y}, peso, {v1, v2}, BLACK};

    grafo->vertices[v1].quant_a++;
    grafo->vertices[v1].arestas[grafo->vertices[v1].quant_a - 1] = &grafo->arestas[grafo->quant_a - 1];
    grafo->vertices[v2].quant_a++;
    grafo->vertices[v2].arestas[grafo->vertices[v2].quant_a - 1] = &grafo->arestas[grafo->quant_a - 1];
}

void criar_grafo(Grafo *grafo)
{
    grafo->vertices = (Vertice *)malloc(sizeof(Vertice) * 10);
    grafo->quant_v = 0;
    grafo->limit[0] = 10;
    grafo->arestas = (Aresta *)malloc(sizeof(Aresta) * 10);
    grafo->quant_a = 0;
    grafo->limit[1] = 10;
}

void desenha_grafo(Grafo *grafo)
{

    for (int i = 0; i < grafo->quant_a; i++)
    {
        Aresta a = grafo->arestas[i];
        DrawLineEx(a.startpos, a.endpos, 5, a.cor);
    }

    for (int i = 0; i < grafo->quant_v; i++)
    {
        Circle v = grafo->vertices[i].v;
        int text_size = MeasureText(TextFormat("%d", i), 20);
        DrawCircle(v.x, v.y, v.radius, v.cor);
        DrawText(TextFormat("%d", i), v.x - text_size / 2, v.y - 10, 20, BLACK);
    }
}

void move_vertice(Grafo *grafo, int vertice_index, int new_x, int new_y)
{
    grafo->vertices[vertice_index].v.x = new_x;
    grafo->vertices[vertice_index].v.y = new_y;

    for (int i = 0; i < grafo->vertices[vertice_index].quant_a; i++)
    {
        Aresta *a = grafo->vertices[vertice_index].arestas[i];
        if (a->vertice[0] == vertice_index)
        {
            a->startpos = (Vector2){new_x, new_y};
        }
        else
        {
            a->endpos = (Vector2){new_x, new_y};
        }
    }
}

int main()
{
    Grafo grafo;
    criar_grafo(&grafo);
    add_vertice(&grafo, 100, 150, GREEN);
    add_vertice(&grafo, 200, 150, GREEN);
    add_vertice(&grafo, 150, 50, GREEN);
    add_vertice(&grafo, 100, 250, GREEN);
    add_vertice(&grafo, 200, 250, GREEN);

    add_aresta(&grafo, 4, 1, 10);
    add_aresta(&grafo, 3, 0, 10);
    add_aresta(&grafo, 3, 4, 10);
    add_aresta(&grafo, 0, 1, 10);
    add_aresta(&grafo, 1, 2, 20);
    add_aresta(&grafo, 0, 2, 30);

    bool moving_vertex = false;
    int vertice_movendo_atual = 0;

    Camera2D camera = {
        (Vector2){.0f, .0f},
        (Vector2){.0f, .0f},
        0,
        1,
    };

    InitWindow(600, 800, "Grafo");
    SetTargetFPS(20);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose())
    {
        Vector2 mousepoint = GetMousePosition();
        Vector2 mouseWorldPos = GetScreenToWorld2D(mousepoint, camera);

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            if (moving_vertex == false)
            {
                for (vertice_movendo_atual = 0; vertice_movendo_atual < grafo.quant_v; vertice_movendo_atual++)
                {
                    if (CheckCollisionPointCircle(mouseWorldPos, (Vector2){grafo.vertices[vertice_movendo_atual].v.x, grafo.vertices[vertice_movendo_atual].v.y}, grafo.vertices[vertice_movendo_atual].v.radius))
                    {
                        moving_vertex = true;
                        break;
                    }
                }
            }

            if (moving_vertex)
            {
                move_vertice(&grafo, vertice_movendo_atual, mouseWorldPos.x, mouseWorldPos.y);
            }
        }

        if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))
        {
            moving_vertex = false;
        }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !moving_vertex)
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Get the world point that is under the mouse

            // Set the offset to where the mouse is
            camera.offset = mousepoint;

            // Set the target to match, so that the camera maps the world space point
            // under the cursor to the screen space point under the cursor at any zoom
            camera.target = mouseWorldPos;

            // Zoom increment
            // Uses log scaling to provide consistent zoom speed
            float scale = 0.2f * wheel;
            camera.zoom = Clamp(expf(logf(camera.zoom) + scale), 0.125f, 64.0f);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        desenha_grafo(&grafo);

        EndMode2D();
        EndDrawing();
    };

    CloseWindow();

    for (int i = 0; i < grafo.quant_v; i++)
    {
        free(grafo.vertices[i].arestas);
    }
    free(grafo.vertices);
    free(grafo.arestas);

    return 0;
}