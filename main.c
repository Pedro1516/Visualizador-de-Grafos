#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include "interface.h"

typedef struct
{
    int *dados;
    int inicio;
    int fim;
    int quant_dados;
} Fila;

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
    int direcionado; // 0 para não direcionado, 1 para direcionado
    int limit[2];    // memoria alocada para v e a. indice 0 para vertice e 1 para aresta
    int ponderado;
} Grafo;

typedef struct
{
    Fila *fila;
    int *visitado;
    int vertice_atual;
    int aresta_index;

    float timer;
    float delay;

    bool ativa;

} BFSAnim;

typedef struct
{
    int *pilha;
    int topo;

    int *visitado;
    int *indice_aresta;

    float timer;
    float delay;

    bool ativa;

} DFSAnim;

Fila *criar_fila(int quant_dados)
{
    Fila *fila = (Fila *)malloc(sizeof(Fila));
    fila->dados = (int *)calloc(quant_dados, sizeof(int));
    fila->quant_dados = 0;
    fila->inicio = 0;
    fila->fim = 0;
    return fila;
}

void enfileirar(Fila *fila, int dado)
{
    fila->dados[fila->fim] = dado;
    fila->fim++;
    fila->quant_dados++;
}

int desenfileirar(Fila *fila)
{
    if (fila->inicio == fila->fim)
        return -1;

    fila->quant_dados--;
    return fila->dados[fila->inicio++];
}

void add_vertice(Grafo *grafo, int posx, int posy, Color color)
{
    if (grafo->quant_v + 1 > grafo->limit[0])
    {
        int novo_limite = (grafo->limit[0] == 0) ? 10 : grafo->limit[0] * 2;

        grafo->vertices = realloc(grafo->vertices, sizeof(Vertice) * novo_limite);

        grafo->limit[0] = novo_limite;
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
        int novo_limite = (grafo->limit[1] == 0) ? 10 : grafo->limit[1] * 2;

        grafo->arestas = realloc(grafo->arestas, sizeof(Aresta) * novo_limite);

        grafo->limit[1] = novo_limite;
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

void criar_grafo(Grafo *grafo, int direcionado, int ponderado)
{
    grafo->vertices = (Vertice *)malloc(sizeof(Vertice) * 10);
    grafo->quant_v = 0;
    grafo->limit[0] = 10;
    grafo->arestas = (Aresta *)malloc(sizeof(Aresta) * 10);
    grafo->quant_a = 0;
    grafo->limit[1] = 10;
    grafo->direcionado = direcionado;
    grafo->ponderado = ponderado;
}

void desenha_grafo(Grafo *grafo, Font font, float zoom)
{
    for (int i = 0; i < grafo->quant_a; i++)
    {
        Aresta *a = &grafo->arestas[i];

        Vector2 start = {
            grafo->vertices[a->vertice[0]].v.x,
            grafo->vertices[a->vertice[0]].v.y};

        Vector2 end = {
            grafo->vertices[a->vertice[1]].v.x,
            grafo->vertices[a->vertice[1]].v.y};

        DrawLineEx(start, end, 2, a->cor);

        if (grafo->direcionado)
        {
            Vector2 direction = Vector2Subtract(end, start);

            if (Vector2Length(direction) < 0.001f)
                continue;

            Vector2 normalized = Vector2Normalize(direction);

            Vector2 tip = Vector2Add(end, Vector2Scale(normalized, -22));

            // calcula o ângulo da aresta
            float angle = atan2f(direction.y, direction.x) * RAD2DEG;

            // compensação correta para DrawPoly
            angle -= 90;

            DrawPoly(tip, 3, 5, angle, BLACK);
        }
    }

    if (zoom > 1.0f && grafo->ponderado)
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
        (*camera).zoom = Clamp(expf(logf((*camera).zoom) + scale), 0.064f, 64.0f);
    }
}

void set_target_camera(Camera2D *camera, bool moving_vertex, bool *moving_camera, Vector2 mousepoint, Button **botoes, int quant_btn)
{
    for (int i = 0; i < quant_btn; i++)
    {
        if (CheckCollisionPointRec(mousepoint, botoes[i]->rect))
        {
            (*moving_camera) = false;
            return;
        }
    }
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

int select_aresta(Grafo *grafo, Vector2 mousepoint, int *aresta_selec)
{
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
    {
        for (int i = 0; i < grafo->quant_a; i++)
        {
            Aresta a = grafo->arestas[i];
            if (CheckCollisionPointLine(mousepoint, a.startpos, a.endpos, 5) && CheckCollisionPointCircle(mousepoint, a.startpos, 20) == false && CheckCollisionPointCircle(mousepoint, a.endpos, 20) == false)
            {
                if (*aresta_selec == i)
                {
                    *aresta_selec = -1;
                    grafo->arestas[i].cor = BLACK;
                    return 0;
                }
                else
                {
                    *aresta_selec = i;
                    return 1;
                }

                break;
            }
        }
    }

    return 0;
}

int select_vertex(Grafo *grafo, Vector2 mousepoint, int *vertices_selec)
{
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
                        return 1;
                    }
                    else
                    {
                        vertices_selec[1] = i;
                        return 1;
                    }
                }

                break;
            }
        }
    }

    if (vertices_selec[0] == -1 || vertices_selec[1] == -1)
        return 0;

    return 1;
}

void desenha_selecao_vertice(Grafo *grafo, int *vertices_selecionados)
{
    if (vertices_selecionados[0] != -1)
    {
        Circle v1 = grafo->vertices[vertices_selecionados[0]].v;
        DrawCircleLines(v1.x, v1.y, v1.radius * 1.2, (Color){0, 0, 150, 255});
    }

    if (vertices_selecionados[1] != -1)
    {
        Circle v2 = grafo->vertices[vertices_selecionados[1]].v;
        DrawCircleLines(v2.x, v2.y, v2.radius * 1.2, (Color){100, 0, 0, 255});
    }
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

void gerar_k_completo(Grafo *grafo, int n, float raio_circulo)
{
    Vector2 centro = {1000, 1000};

    for (int i = 0; i < n; i++)
    {
        float theta = i * (2.0f * PI / n);
        float x = centro.x + raio_circulo * cosf(theta);
        float y = centro.y + raio_circulo * sinf(theta);
        add_vertice(grafo, x, y, GREEN);
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            add_aresta(grafo, i, j, GetRandomValue(1, 100));
        }
    }
}

int add_aresta_selec(Grafo *grafo, int *vertices_selec)
{
    int v1 = vertices_selec[0];
    int v2 = vertices_selec[1];

    if (v1 == -1 || v2 == -1)
        return 0;

    // verifica se já existe exatamente v1 -> v2
    for (int i = 0; i < grafo->vertices[v1].quant_a; i++)
    {
        int a_idx = grafo->vertices[v1].arestas[i];
        Aresta *a = &grafo->arestas[a_idx];

        if (a->vertice[0] == v1 && a->vertice[1] == v2)
        {
            vertices_selec[0] = -1;
            vertices_selec[1] = -1;
            return 0;
        }

        // se o grafo não for direcionado bloqueia também v2->v1
        if (!grafo->direcionado)
        {
            if (a->vertice[0] == v2 && a->vertice[1] == v1)
            {
                vertices_selec[0] = -1;
                vertices_selec[1] = -1;
                return 0;
            }
        }
    }

    add_aresta(grafo, v1, v2, 0);

    vertices_selec[0] = -1;
    vertices_selec[1] = -1;

    return 1;
}

void iniciar_dfs(Grafo *grafo, DFSAnim *dfs, int origem)
{
    dfs->pilha = malloc(sizeof(int) * grafo->quant_v);
    dfs->visitado = calloc(grafo->quant_v, sizeof(int));
    dfs->indice_aresta = calloc(grafo->quant_v, sizeof(int));

    dfs->topo = 0;
    dfs->pilha[dfs->topo] = origem;

    dfs->visitado[origem] = 1;

    dfs->timer = 0;
    dfs->delay = 0.3f;

    dfs->ativa = true;
}

void iniciar_bfs(Grafo *grafo, BFSAnim *bfs, int origem)
{
    bfs->fila = criar_fila(grafo->quant_v);
    bfs->visitado = calloc(grafo->quant_v, sizeof(int));

    bfs->visitado[origem] = 1;
    enfileirar(bfs->fila, origem);

    bfs->vertice_atual = -1;
    bfs->aresta_index = 0;

    bfs->timer = 0;
    bfs->delay = 0.3f;

    bfs->ativa = true;
}

void update_dfs(Grafo *grafo, DFSAnim *dfs)
{
    if (!dfs->ativa)
        return;

    dfs->timer += GetFrameTime();

    if (dfs->timer < dfs->delay)
        return;

    dfs->timer = 0;

    if (dfs->topo < 0)
    {
        dfs->ativa = false;
        return;
    }

    int atual = dfs->pilha[dfs->topo];
    Vertice *v = &grafo->vertices[atual];

    if (dfs->indice_aresta[atual] >= v->quant_a)
    {
        dfs->topo--; // backtrack
        return;
    }

    int aresta_idx = v->arestas[dfs->indice_aresta[atual]];
    dfs->indice_aresta[atual]++;

    Aresta *a = &grafo->arestas[aresta_idx];
    int outro;
    if (grafo->direcionado)
    {
        if (a->vertice[0] != atual)
        {
            return;
        }

        outro = a->vertice[1];
    }
    else
    {
        outro = (a->vertice[0] == atual) ? a->vertice[1] : a->vertice[0];
    }

    if (!dfs->visitado[outro])
    {
        dfs->visitado[outro] = 1;

        a->cor = BLUE; // cor da DFS

        dfs->topo++;
        dfs->pilha[dfs->topo] = outro;
    }
}

void update_bfs(Grafo *grafo, BFSAnim *bfs)
{
    if (!bfs->ativa)
        return;

    bfs->timer += GetFrameTime();

    if (bfs->timer < bfs->delay)
        return;

    bfs->timer = 0;

    if (bfs->vertice_atual == -1)
    {
        if (bfs->fila->quant_dados == 0)
        {
            bfs->ativa = false;
            return;
        }

        bfs->vertice_atual = desenfileirar(bfs->fila);
        bfs->aresta_index = 0;
    }

    Vertice *v = &grafo->vertices[bfs->vertice_atual];

    if (bfs->aresta_index >= v->quant_a)
    {
        bfs->vertice_atual = -1;
        return;
    }

    int aresta_idx = v->arestas[bfs->aresta_index];
    Aresta *a = &grafo->arestas[aresta_idx];

    int outro;
    if (grafo->direcionado)
    {
        if (a->vertice[0] != bfs->vertice_atual)
        {
            bfs->aresta_index++;
            return;
        }

        outro = a->vertice[1];
    }
    else
    {
        outro = (a->vertice[0] == bfs->vertice_atual) ? a->vertice[1] : a->vertice[0];
    }

    if (!bfs->visitado[outro])
    {
        bfs->visitado[outro] = 1;
        a->cor = RED;
        enfileirar(bfs->fila, outro);
    }

    bfs->aresta_index++;
}

int limpar_animacao_bfs(BFSAnim *bfs, Grafo *grafo)
{
    for (int i = 0; i < grafo->quant_a; i++)
    {
        if (grafo->arestas[i].cor.r != 0)
        {
            grafo->arestas[i].cor = BLACK;
        }
    }

    free(bfs->fila->dados);
    free(bfs->fila);
    free(bfs->visitado);
    bfs->ativa = false;
    return 1;
}

int limpar_animacao_dfs(DFSAnim *dfs, Grafo *grafo)
{
    for (int i = 0; i < grafo->quant_a; i++)
    {
        if (grafo->arestas[i].cor.b != 0)
        {
            grafo->arestas[i].cor = BLACK;
        }
    }

    free(dfs->pilha);
    free(dfs->visitado);
    dfs->ativa = false;
    return 1;
}

void limpar_animacao(Grafo *grafo, BFSAnim *bfs, DFSAnim *dfs)
{
    if (bfs->ativa)
        limpar_animacao_bfs(bfs, grafo);

    if (dfs->ativa)
        limpar_animacao_dfs(dfs, grafo);
}

int main()
{
    int quant_btn = 8;
    int screen_w = 1280;
    int screen_h = 720;

    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    Button **botoes = (Button **)malloc(sizeof(Button *) * quant_btn);
    criar_grafo(grafo, 1, 1);
    BFSAnim bfs_anim;
    bfs_anim.ativa = false;
    DFSAnim dfs_anim;
    dfs_anim.ativa = false;

    bool input_ativo_edicao_aresta = false;

    MenuEdicaoAresta menu_edicao_aresta = {
        .rect_menu = (Rectangle){
            .height = 100,
            .width = 200,
            .x = screen_w - 470,
            .y = 360},
        .ativa = false,
    };

    menu_edicao_aresta.input.buffer = (char *)malloc(sizeof(char) * 6);
    menu_edicao_aresta.input.buffer[5] = '\0';
    menu_edicao_aresta.input.limit_char = 5;
    menu_edicao_aresta.input.char_inserted = 0;

    bool moving_vertex = false;
    bool moving_cam = false;
    int vertice_movendo_atual = 0;
    int vertices_selecionados[2] = {-1, -1};
    int aresta_selecionada = -1;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_w, screen_h, "Grafo");
    SetTargetFPS(60);

    Camera2D camera = {
        (Vector2){.0f, .0f},
        (Vector2){.0f, .0f},
        0,
        1,
    };

    Font font = LoadFont("docs/fonts/Oswald.ttf");
    botoes[0] = create_button_rect((Rectangle){GetScreenWidth() - 250, 30, 150, 50}, RED, "Criar Vertice");
    botoes[1] = create_button_rect((Rectangle){GetScreenWidth() - 250, 130, 150, 50}, RED, "Criar Aresta");
    botoes[2] = create_button_rect((Rectangle){GetScreenWidth() - 250, 230, 150, 50}, RED, "Excluir");
    botoes[3] = create_button_rect((Rectangle){GetScreenWidth() - 250, 330, 150, 50}, RED, "Editar Peso");
    botoes[4] = create_button_rect((Rectangle){GetScreenWidth() - 250, 430, 150, 50}, RED, "BFS");
    botoes[5] = create_button_rect((Rectangle){GetScreenWidth() - 250, 530, 150, 50}, RED, "DFS");
    botoes[6] = create_button_rect((Rectangle){GetScreenWidth() - 250, 630, 150, 50}, RED, "Limpar Animação");
    botoes[7] = create_button_rect((Rectangle){GetScreenWidth() - 250, 730, 150, 50}, RED, "Criar K Completo");

    gerar_k_completo(grafo, 10, 500.0f);

    while (!WindowShouldClose())
    {
        Vector2 mousepoint = GetMousePosition();
        Vector2 mouseWorldPos = GetScreenToWorld2D(mousepoint, camera);

        if (moving_cam == false)
            check_collision_vertex(grafo, &moving_vertex, mouseWorldPos, &vertice_movendo_atual);

        if (IsWindowResized())
            for (size_t i = 0; i < quant_btn; i++)
            {
                botoes[i]->rect.x = GetScreenWidth() - 150;
            }

        set_target_camera(&camera, moving_vertex, &moving_cam, mousepoint, botoes, quant_btn);
        set_zoom(&camera, mousepoint, mouseWorldPos);

        if (!moving_cam)
        {
            int aresta_anterior = aresta_selecionada;

            if (select_vertex(grafo, mouseWorldPos, vertices_selecionados))
            {
                if (aresta_selecionada != -1)
                {
                    grafo->arestas[aresta_selecionada].cor = BLACK;
                    aresta_selecionada = -1;
                }
            }

            if (select_aresta(grafo, mouseWorldPos, &aresta_selecionada) == 1)
            {
                if (aresta_anterior != -1)
                    grafo->arestas[aresta_anterior].cor = BLACK;
                grafo->arestas[aresta_selecionada].cor = RED;

                if (vertices_selecionados[0] != -1)
                    vertices_selecionados[0] = -1;
                if (vertices_selecionados[1] != -1)
                    vertices_selecionados[1] = -1;
            }
        }

        if (onButtonClick(botoes[0], mousepoint) && !moving_cam) // Add vertice
        {
            add_vertice(grafo, 100, 150, GREEN);
        }

        if (onButtonClick(botoes[1], mousepoint) && !moving_cam) // Add aresta
        {
            add_aresta_selec(grafo, vertices_selecionados);
            vertices_selecionados[0] = -1;
            vertices_selecionados[1] = -1;
        }

        if (onButtonClick(botoes[2], mousepoint) && !moving_cam) // Excluir vertice ou aresta
        {
            if (aresta_selecionada != -1)
            {
                excluir_aresta(grafo, aresta_selecionada);
                aresta_selecionada = -1;
            }

            if (vertices_selecionados[0] != -1)
            {
                excluir_vertice(grafo, vertices_selecionados[0]);
                vertices_selecionados[0] = -1;
            }
        }

        if (onButtonClick(botoes[3], mousepoint) && aresta_selecionada != -1 && !moving_cam) // Editar peso
        {
            menu_edicao_aresta.ativa = true;
            menu_edicao_aresta.rect_menu.x = GetScreenWidth() - 470;
        }

        if (onButtonClick(botoes[4], mousepoint) && vertices_selecionados[0] != -1 && !moving_cam) // bfs
        {
            limpar_animacao(grafo, &bfs_anim, &dfs_anim);

            iniciar_bfs(grafo, &bfs_anim, vertices_selecionados[0]);
            vertices_selecionados[0] = -1;
        }

        if (onButtonClick(botoes[5], mousepoint) && vertices_selecionados[0] != -1 && !moving_cam) // dfs
        {
            limpar_animacao(grafo, &bfs_anim, &dfs_anim);

            iniciar_dfs(grafo, &dfs_anim, vertices_selecionados[0]);
            vertices_selecionados[0] = -1;
        }

        if (onButtonClick(botoes[6], mousepoint) && !moving_cam) // limpar animacao
        {
            limpar_animacao(grafo, &bfs_anim, &dfs_anim);
        }

        if (onButtonClick(botoes[7], mousepoint) && !moving_cam) // gerar k completo
        {
            free(grafo->vertices);
            free(grafo->arestas);
            criar_grafo(grafo, 0, 0);
            int n = 120;
            gerar_k_completo(grafo, n, 50.f * n);
        }

        if (menu_edicao_aresta.ativa)
        {
            int key = 0;
            int *char_inserted = &menu_edicao_aresta.input.char_inserted;
            key = GetCharPressed();
            while (key != 0)
            {
                if (key >= 48 && key <= 57 && (*char_inserted) < menu_edicao_aresta.input.limit_char)
                {
                    menu_edicao_aresta.input.buffer[*char_inserted] = key;
                    (*char_inserted)++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                if ((*char_inserted) > 0)
                {
                    menu_edicao_aresta.input.buffer[(*char_inserted) - 1] = '\0';
                    (*char_inserted)--;
                }
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                grafo->arestas[aresta_selecionada].peso = atoi(menu_edicao_aresta.input.buffer);
                (*char_inserted) = 0;
                menu_edicao_aresta.ativa = false;
                grafo->arestas[aresta_selecionada].cor = BLACK;
                aresta_selecionada = -1;

                for (size_t i = 0; i < menu_edicao_aresta.input.limit_char; i++)
                    menu_edicao_aresta.input.buffer[i] = 0;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

        desenha_grafo(grafo, font, camera.zoom);
        desenha_selecao_vertice(grafo, vertices_selecionados);
        update_bfs(grafo, &bfs_anim);
        update_dfs(grafo, &dfs_anim);

        EndMode2D();

        if (menu_edicao_aresta.ativa)
        {
            desenha_menu_edicao_aresta(&menu_edicao_aresta);
            DrawText(menu_edicao_aresta.input.buffer, menu_edicao_aresta.rect_menu.x + 90, menu_edicao_aresta.rect_menu.y + 45, 20, BLACK);
        }

        for (size_t i = 0; i < quant_btn; i++)
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
    free(menu_edicao_aresta.input.buffer);

    return 0;
}