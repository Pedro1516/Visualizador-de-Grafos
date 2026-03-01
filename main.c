#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include "interface.h"

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
    int *arestas;
    int quant_a;
    int limit_a;
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
    grafo->vertices[grafo->quant_v - 1].quant_a = 0;
    grafo->vertices[grafo->quant_v - 1].limit_a = 5;
    grafo->vertices[grafo->quant_v - 1].arestas = (int *)malloc(sizeof(int) * 5);
}

void add_aresta(Grafo *grafo, int v1, int v2, int peso)
{
    if (grafo->quant_a + 1 > grafo->limit[1])
    {
        grafo->arestas = (Aresta *)realloc(grafo->arestas, sizeof(Aresta) * ((grafo->limit[1] == 0) ? 10 : grafo->limit[1] * 2));
        grafo->limit[1] += 10;
    }

    grafo->quant_a++;
    grafo->arestas[grafo->quant_a - 1] = (Aresta){(Vector2){grafo->vertices[v1].v.x, grafo->vertices[v1].v.y}, (Vector2){grafo->vertices[v2].v.x, grafo->vertices[v2].v.y}, peso, {v1, v2}, BLACK};

    if (grafo->vertices[v1].quant_a + 1 > grafo->vertices[v1].limit_a)
    {
        grafo->vertices[v1].arestas = (int *)realloc(grafo->vertices[v1].arestas, sizeof(int) * (grafo->vertices[v1].limit_a + 5));
        grafo->vertices[v1].limit_a += 5;
    }

    grafo->vertices[v1].quant_a++;
    grafo->vertices[v1].arestas[grafo->vertices[v1].quant_a - 1] = grafo->quant_a - 1;

    if (grafo->vertices[v2].quant_a + 1 > grafo->vertices[v2].limit_a)
    {
        grafo->vertices[v2].arestas = (int *)realloc(grafo->vertices[v2].arestas, sizeof(int) * (grafo->vertices[v2].limit_a + 5));
        grafo->vertices[v2].limit_a += 5;
    }

    grafo->vertices[v2].quant_a++;
    grafo->vertices[v2].arestas[grafo->vertices[v2].quant_a - 1] = grafo->quant_a - 1;
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

void desenha_grafo(Grafo *grafo, Font font, float zoom)
{

    if (zoom > 0.2f)
    {
        for (int i = 0; i < grafo->quant_a; i++)
        {
            Aresta a = grafo->arestas[i];
            DrawLineEx(a.startpos, a.endpos, 1, a.cor);
        }
    }

    if (zoom > 1.0f)
    {
        for (int i = 0; i < grafo->quant_a; i++)
        {
            Aresta a = grafo->arestas[i];
            Vector2 pos = {(a.startpos.x + a.endpos.x) / 2, (a.startpos.y + a.endpos.y) / 2};

            float dy = a.endpos.y - a.startpos.y;
            float dx = a.endpos.x - a.startpos.x;
            float angle = atan2f(dy, dx) * (180.0f / PI);

            if (angle > 90)
                angle -= 180;
            if (angle < -90)
                angle += 180;

            DrawTextPro(font, TextFormat("%d", a.peso), pos, (Vector2){0, 15}, angle, 20, 2.0f, RED);
        }
    }

    for (int i = 0; i < grafo->quant_v; i++)
    {
        Circle v = grafo->vertices[i].v;
        int text_size = MeasureText(TextFormat("%d", i), 20);
        DrawCircle(v.x, v.y, v.radius, v.color);
        DrawText(TextFormat("%d", i), v.x - text_size / 2, v.y - 10, 20, BLACK);
    }
}

void move_vertice(Grafo *grafo, int vertice_index, int new_x, int new_y)
{
    grafo->vertices[vertice_index].v.x = new_x;
    grafo->vertices[vertice_index].v.y = new_y;

    for (int i = 0; i < grafo->vertices[vertice_index].quant_a; i++)
    {
        Aresta *a = &grafo->arestas[grafo->vertices[vertice_index].arestas[i]];
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

void set_zoom(Camera2D *camera, Vector2 mousepoint, Vector2 mouseWorldPos)
{

    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse

        // Set the offset to where the mouse is
        (*camera).offset = mousepoint;

        // Set the target to match, so that the camera maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        (*camera).target = mouseWorldPos;

        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        float scale = 0.2f * wheel;
        (*camera).zoom = Clamp(expf(logf((*camera).zoom) + scale), 0.125f, 64.0f);
    }
}

void set_target_camera(Camera2D *camera, bool moving_vertex, bool *moving_camera)
{
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !moving_vertex)
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / (*camera).zoom);
        (*camera).target = Vector2Add((*camera).target, delta);

        (*moving_camera) = true;
    }

    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))
        (*moving_camera) = false;
}

void check_collision_vertex(Grafo *grafo, bool *moving_vertex, Vector2 mouseWorldPos, int *vertice_movendo_atual)
{
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        if ((*moving_vertex) == false)
        {
            for ((*vertice_movendo_atual) = 0; (*vertice_movendo_atual) < grafo->quant_v; (*vertice_movendo_atual)++)
            {
                if (CheckCollisionPointCircle(mouseWorldPos, (Vector2){grafo->vertices[(*vertice_movendo_atual)].v.x, grafo->vertices[(*vertice_movendo_atual)].v.y}, grafo->vertices[(*vertice_movendo_atual)].v.radius))
                {
                    (*moving_vertex) = true;
                    break;
                }
            }
        }

        if ((*moving_vertex))
        {
            move_vertice(grafo, (*vertice_movendo_atual), mouseWorldPos.x, mouseWorldPos.y);
        }
    }

    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))
    {
        (*moving_vertex) = false;
    }
}

int select_vertex(Grafo *grafo, Vector2 mousepoint, int *vertices_selec)
{
    if (vertices_selec[0] != -1 && vertices_selec[1] != -1)
    {
        add_aresta(grafo, vertices_selec[0], vertices_selec[1], 0);
        return 1;
    }

    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
    {
        for (int i = 0; i < grafo->quant_v; i++)
        {
            Circle circle = grafo->vertices[i].v;
            if (CheckCollisionPointCircle(mousepoint, (Vector2){circle.x, circle.y}, circle.radius))
            {
                if (i == vertices_selec[0])
                {
                    vertices_selec[0] = -1;
                }
                else if (i == vertices_selec[1])
                {
                    vertices_selec[1] = -1;
                }
                else
                {
                    if (vertices_selec[0] == -1)
                    {
                        vertices_selec[0] = i;
                    }
                    else
                    {
                        vertices_selec[1] = i;
                    }
                }

                break;
            }
        }
    }

    if (vertices_selec[0] == -1 || vertices_selec[1] == -1)
        return 0;

    for (size_t i = 0; i < grafo->vertices[vertices_selec[0]].quant_a; i++)
    {
        for (size_t j = 0; j < grafo->vertices[vertices_selec[1]].quant_a; j++)
        {
            if (grafo->vertices[vertices_selec[0]].arestas[i] == grafo->vertices[vertices_selec[1]].arestas[j])
            {
                vertices_selec[0] = -1;
                vertices_selec[1] = -1;
                return 0;
            }
        }
    }

    return 0;
}

void desenha_selecao_vertice(Grafo *grafo, int *vertices_selecionados)
{
    Circle v1 = grafo->vertices[vertices_selecionados[0]].v;
    DrawCircleLines(v1.x, v1.y, v1.radius * 1.2, BLACK);

    Circle v2 = grafo->vertices[vertices_selecionados[1]].v;
    DrawCircleLines(v2.x, v2.y, v2.radius * 1.2, BLACK);
}

void excluir_aresta(Grafo *grafo, int aresta_idx)
{
    if (aresta_idx < 0 || aresta_idx >= grafo->quant_a)
        return;

    for (int v = 0; v < grafo->quant_v; v++)
    {
        for (int i = 0; i < grafo->vertices[v].quant_a; i++)
        {
            if (grafo->vertices[v].arestas[i] == aresta_idx)
            {

                for (int j = i; j < grafo->vertices[v].quant_a - 1; j++)
                {
                    grafo->vertices[v].arestas[j] = grafo->vertices[v].arestas[j + 1];
                }
                grafo->vertices[v].quant_a--;
                break;
            }
        }
    }

    for (int i = aresta_idx; i < grafo->quant_a - 1; i++)
    {
        grafo->arestas[i] = grafo->arestas[i + 1];
    }
    grafo->quant_a--;

    for (int v = 0; v < grafo->quant_v; v++)
    {
        for (int i = 0; i < grafo->vertices[v].quant_a; i++)
        {
            if (grafo->vertices[v].arestas[i] > aresta_idx)
            {
                grafo->vertices[v].arestas[i]--;
            }
        }
    }
}

void excluir_vertice(Grafo *grafo, int vertice_idx)
{
    if (vertice_idx < 0 || vertice_idx >= grafo->quant_v)
        return;

    int i = 0;
    while (i < grafo->vertices[vertice_idx].quant_a)
    {
        excluir_aresta(grafo, grafo->vertices[vertice_idx].arestas[i]);
    }

    free(grafo->vertices[vertice_idx].arestas);

    for (int i = vertice_idx; i < grafo->quant_v - 1; i++)
    {
        grafo->vertices[i] = grafo->vertices[i + 1];
    }
    grafo->quant_v--;

    for (int a = 0; a < grafo->quant_a; a++)
    {
        if (grafo->arestas[a].vertice[0] > vertice_idx)
            grafo->arestas[a].vertice[0]--;
        if (grafo->arestas[a].vertice[1] > vertice_idx)
            grafo->arestas[a].vertice[1]--;
    }
}


void gerar_k_completo(Grafo *grafo, int n) {
    float raio_circulo = 1000.0f;
    Vector2 centro = { 1000, 1000 };

    for (int i = 0; i < n; i++) {
        float theta = i * (2.0f * PI / n);
        float x = centro.x + raio_circulo * cosf(theta);
        float y = centro.y + raio_circulo * sinf(theta);
        add_vertice(grafo, x, y, GREEN);
    }

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            add_aresta(grafo, i, j, GetRandomValue(1, 100));
        }
    }
}

int main()
{
    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    Button **botoes = (Button **)malloc(sizeof(Button *) * 5);
    criar_grafo(grafo);

    // add_vertice(grafo, 100, 150, GREEN);
    // add_vertice(grafo, 200, 150, GREEN);
    // add_vertice(grafo, 150, 50, GREEN);
    // add_vertice(grafo, 100, 250, GREEN);
    // add_vertice(grafo, 200, 250, GREEN);

    // add_aresta(grafo, 0, 1, 10);
    // add_aresta(grafo, 0, 2, 10);
    // add_aresta(grafo, 0, 3, 10);
    // add_aresta(grafo, 0, 4, 10);
    // add_aresta(grafo, 1, 2, 20);
    // add_aresta(grafo, 1, 3, 30);
    // add_aresta(grafo, 1, 4, 30);
    // add_aresta(grafo, 2, 3, 30);
    // add_aresta(grafo, 2, 4, 30);
    // add_aresta(grafo, 3, 4, 30);

    bool moving_vertex = false;
    bool moving_cam = false;
    int vertice_movendo_atual = 0;
    int vertices_selecionados[2] = {-1, -1};

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(600, 800, "Grafo");
    SetTargetFPS(60);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    Camera2D camera = {
        (Vector2){.0f, .0f},
        (Vector2){.0f, .0f},
        0,
        1,
    };

    Font font = LoadFont("fonts/Oswald.ttf");
    botoes[0] = create_button_rect((Rectangle){GetScreenWidth() - 150, 150, 100, 50}, RED, "Criar Vertice");
    botoes[1] = create_button_rect((Rectangle){GetScreenWidth() - 150, 230, 100, 50}, RED, "Excluir");
    botoes[2] = create_button_rect((Rectangle){GetScreenWidth() - 150, 310, 100, 50}, RED, "Editar Peso");


    gerar_k_completo(grafo, 5);

    while (!WindowShouldClose())
    {
        Vector2 mousepoint = GetMousePosition();
        Vector2 mouseWorldPos = GetScreenToWorld2D(mousepoint, camera);

        if (moving_cam == false)
            check_collision_vertex(grafo, &moving_vertex, mouseWorldPos, &vertice_movendo_atual);

        set_target_camera(&camera, moving_vertex, &moving_cam);
        set_zoom(&camera, mousepoint, mouseWorldPos);

        if (!moving_cam)
        {
            if (onButtonClick(botoes[0], mousepoint)) // Add vertice
            {
                add_vertice(grafo, 100, 150, GREEN);
            }

            if (onButtonClick(botoes[1], mousepoint) && vertices_selecionados[0] != -1) // Excluir vertice
            {
                excluir_vertice(grafo, vertices_selecionados[0]);
                vertices_selecionados[0] = -1;
            }
        }

        if (select_vertex(grafo, mouseWorldPos, vertices_selecionados))
        {
            vertices_selecionados[0] = -1;
            vertices_selecionados[1] = -1;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

        desenha_grafo(grafo, font, camera.zoom);
        desenha_selecao_vertice(grafo, vertices_selecionados);

        EndMode2D();

        for (size_t i = 0; i < 3; i++)
        {
            drawButton(botoes[i], 15);
        }

        DrawText(TextFormat("Vertices: %d", grafo->quant_v), 10, 10, 20, BLACK);
        DrawText(TextFormat("Arestas: %d", grafo->quant_a), 10, 40, 20, BLACK);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 70, 20, BLACK);

        EndDrawing();
    };

    CloseWindow();

    for (int i = 0; i < grafo->quant_v; i++)
    {
        free(grafo->vertices[i].arestas);
    }
    free(grafo->vertices);
    free(grafo->arestas);

    return 0;
}