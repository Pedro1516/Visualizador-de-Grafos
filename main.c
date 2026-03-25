#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include "interface.h"
#include <emscripten.h>

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

Grafo *grafo_global;

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

void desenha_grafo(Grafo *grafo, Font font, float zoom, bool theme)
{
    Color cor_aresta = BLACK;

    if (theme)
        cor_aresta = WHITE;

    for (int i = 0; i < grafo->quant_a; i++)
    {
        Aresta *a = &grafo->arestas[i];

        if ((a->cor.b == 0 && a->cor.r == 0 && a->cor.g == 0) || (a->cor.b == 255 && a->cor.r == 255 && a->cor.g == 255)) // Preto ou Branco
            a->cor = cor_aresta;

        Vector2 start = {
            grafo->vertices[a->vertice[0]].v.x,
            grafo->vertices[a->vertice[0]].v.y};

        Vector2 end = {
            grafo->vertices[a->vertice[1]].v.x,
            grafo->vertices[a->vertice[1]].v.y};

        Vector2 direction = Vector2Subtract(end, start);
        float len = Vector2Length(direction);

        if (len > 0.001f)
        {
            Vector2 normalized = Vector2Normalize(direction);
            Vector2 perp = (Vector2){-normalized.y, normalized.x};

            // verifica se existe aresta contrária
            int existe_contraria = 0;

            for (int j = 0; j < grafo->vertices[a->vertice[1]].quant_a; j++)
            {
                int idx = grafo->vertices[a->vertice[1]].arestas[j];
                Aresta *b = &grafo->arestas[idx];

                if (b->vertice[0] == a->vertice[1] &&
                    b->vertice[1] == a->vertice[0])
                {
                    existe_contraria = 1;
                    break;
                }
            }

            if (existe_contraria)
            {
                float offset = 8;

                start = Vector2Add(start, Vector2Scale(perp, offset));
                end = Vector2Add(end, Vector2Scale(perp, offset));
            }
        }

        if (a->vertice[0] == a->vertice[1])
        {
            Vector2 center = {
                grafo->vertices[a->vertice[0]].v.x,
                grafo->vertices[a->vertice[0]].v.y};

            float r = grafo->vertices[a->vertice[0]].v.radius;

            Vector2 p1 = {center.x - r * 0.5f, center.y - r * 0.85f};
            Vector2 p2 = {center.x + r * 0.5f, center.y - r * 0.85f};
            Vector2 c1 = {center.x - r * 2.0f, center.y - r * 3.5f};
            Vector2 c2 = {center.x + r * 2.0f, center.y - r * 3.5f};

            Vector2 pontos[4] = {p1, c1, c2, p2};
            DrawSplineBezierCubic(pontos, 4, 2.0f, a->cor);

            if (grafo->direcionado)
            {
                float angle = atan2f(p2.y - c2.y, p2.x - c2.x) * RAD2DEG - 90.0f;
                DrawPoly(p2, 3, 5, angle, a->cor);
            }

            a->startpos = p1;
            a->endpos = p2;
        }
        else
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

            DrawPoly(tip, 3, 5, angle, a->cor);
        }

        a->startpos = start;
        a->endpos = end;
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

            DrawTextPro(font, TextFormat("%d", a.peso), pos, (Vector2){0, 20}, angle, 20, 2.0f, RED);
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

void set_target_camera(Camera2D *camera, bool moving_vertex, bool *moving_camera, Vector2 mousepoint, Button *botoes, int quant_btn)
{
    for (int i = 0; i < quant_btn; i++)
    {
        if (CheckCollisionPointRec(mousepoint, botoes[i].rect))
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

    if (v2 == -1)
        v2 = v1;

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

void update_dfs(Grafo *grafo, DFSAnim *dfs)
{
    if (!dfs->ativa)
        return;

    dfs->timer += GetFrameTime();
    if (dfs->timer < dfs->delay)
        return;

    // Processa arestas sem consumir delay até achar progresso real
    while (true)
    {
        if (dfs->topo < 0)
        {
            dfs->ativa = false;
            return;
        }

        int atual = dfs->pilha[dfs->topo];
        Vertice *v = &grafo->vertices[atual];

        if (dfs->indice_aresta[atual] >= v->quant_a)
        {
            dfs->topo--;
            continue; // backtrack imediato, sem custar delay
        }

        int aresta_idx = v->arestas[dfs->indice_aresta[atual]];
        Aresta *a = &grafo->arestas[aresta_idx];
        dfs->indice_aresta[atual]++;

        int outro;
        if (grafo->direcionado)
        {
            if (a->vertice[0] != atual)
                continue; // aresta de entrada, pula
            outro = a->vertice[1];
        }
        else
        {
            outro = (a->vertice[0] == atual) ? a->vertice[1] : a->vertice[0];
        }

        if (!dfs->visitado[outro])
        {
            dfs->visitado[outro] = 1;
            a->cor = BLUE;
            dfs->pilha[++dfs->topo] = outro;
            dfs->timer = 0; // só consome o delay quando visita alguém novo
            return;
        }
        // já visitado → continua o loop sem custar delay
    }
}

int limpar_animacao_dfs(DFSAnim *dfs, Grafo *grafo)
{
    free(dfs->pilha);
    free(dfs->visitado);
    free(dfs->indice_aresta); /* correção: array que estava vazando */

    dfs->pilha = NULL;
    dfs->visitado = NULL;
    dfs->indice_aresta = NULL;

    dfs->ativa = false;
    return 1;
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
    free(bfs->fila->dados);
    free(bfs->fila);
    free(bfs->visitado);
    bfs->ativa = false;
    return 1;
}

void limpar_animacao(Grafo *grafo, BFSAnim *bfs, DFSAnim *dfs)
{
    if (bfs->ativa)
        limpar_animacao_bfs(bfs, grafo);

    if (dfs->ativa)
        limpar_animacao_dfs(dfs, grafo);

    for (int i = 0; i < grafo->quant_a; i++)
    {
        if (grafo->arestas[i].cor.r != 0 || grafo->arestas[i].cor.b != 0)
        {
            grafo->arestas[i].cor = BLACK;
        }
    }
}

void destroy_grafo(Grafo *grafo)
{
    if (!grafo)
        return;

    for (int i = 0; i < grafo->quant_v; i++)
        free(grafo->vertices[i].arestas);

    free(grafo->arestas);
    free(grafo->vertices);
    free(grafo);
}

void ler_entrada_menu(InputText *input)
{
    int key = 0;
    int *char_inserted = &input->char_inserted;
    key = GetCharPressed();
    while (key != 0)
    {
        if (key >= 48 && key <= 57 && (*char_inserted) < input->limit_char)
        {
            input->buffer[*char_inserted] = key;
            (*char_inserted)++;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        if ((*char_inserted) > 0)
        {
            input->buffer[(*char_inserted) - 1] = '\0';
            (*char_inserted)--;
        }
    }
}

void fechar_menu(MenuEdicao *menu, Vector2 mousepoint)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        if (!CheckCollisionPointRec(mousepoint, menu->rect_menu)) // Clicou em algum lugar que não é menu
            menu->ativa = false;
}

void fechar_menu_rgb(MenuRGB *menu, Vector2 mousepoint)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        if (!CheckCollisionPointRec(mousepoint, menu->rect_menu)) // Clicou em algum lugar que não é menu
            menu->aberto = false;
}

int existe_aresta(Grafo *grafo, int v1, int v2)
{
    for (int i = 0; i < grafo->quant_a; i++)
    {
        Aresta *a = &grafo->arestas[i];

        if (a->vertice[0] == v1 && a->vertice[1] == v2)
            return 1;

        if (!grafo->direcionado)
            if (a->vertice[0] == v2 && a->vertice[1] == v1)
                return 1;
    }
    return 0;
}

void exportar_lista_adj(Grafo *grafo)
{
    int capacidade = 4096;
    int offset = 0;
    char *buffer = malloc(capacidade);

#define ENSURE(n)                             \
    while (offset + (n) >= capacidade)        \
    {                                         \
        capacidade *= 2;                      \
        buffer = realloc(buffer, capacidade); \
    }

    ENSURE(64);
    offset += snprintf(buffer + offset, capacidade - offset,
                       "%d %d %d\n", grafo->quant_v, grafo->ponderado, grafo->direcionado);

    for (int i = 0; i < grafo->quant_v; i++)
    {
        Vertice *v = &grafo->vertices[i];
        int primeiro = 1;

        for (int j = 0; j < v->quant_a; j++)
        {
            int a_idx = v->arestas[j];
            Aresta *a = &grafo->arestas[a_idx];

            int outro;
            if (grafo->direcionado)
            {
                if (a->vertice[0] != i)
                    continue;
                outro = a->vertice[1];
            }
            else
            {
                outro = (a->vertice[0] == i) ? a->vertice[1] : a->vertice[0];
            }

            ENSURE(32);
            if (!primeiro)
                offset += snprintf(buffer + offset, capacidade - offset, " ");

            if (grafo->ponderado)
                offset += snprintf(buffer + offset, capacidade - offset, "%d:%d", outro, a->peso);
            else
                offset += snprintf(buffer + offset, capacidade - offset, "%d", outro);

            primeiro = 0;
        }

        ENSURE(4);
        offset += snprintf(buffer + offset, capacidade - offset, "\n");
    }

    EM_ASM({
        var conteudo = UTF8ToString($0);
        downloadGrafo(conteudo); }, buffer);

    free(buffer);
#undef ENSURE
}

void importar_lista_adj_str(Grafo *grafo, const char *conteudo)
{
    int quant_v, ponderado, direcionado;
    int offset = 0;

    sscanf(conteudo, "%d %d %d%n", &quant_v, &ponderado, &direcionado, &offset);
    conteudo += offset;

    // pula tudo até o fim da linha do cabeçalho
    while (*conteudo && *conteudo != '\n')
        conteudo++;
    if (*conteudo == '\n')
        conteudo++;

    grafo->ponderado = ponderado;
    grafo->direcionado = direcionado;

    for (int i = 0; i < quant_v; i++)
    {
        float ang = i * (2 * PI / quant_v);
        float x = 600 + 200 * cosf(ang);
        float y = 400 + 200 * sinf(ang);
        add_vertice(grafo, x, y, GREEN);
    }

    for (int v = 0; v < quant_v && *conteudo; v++)
    {
        while (*conteudo && *conteudo != '\n')
        {
            // pula espaços, tabs e \r
            while (*conteudo == ' ' || *conteudo == '\t' || *conteudo == '\r')
                conteudo++;

            if (*conteudo == '\n' || *conteudo == '\0')
                break;

            int adj = 0, peso = 0, lido = 0;

            if (grafo->ponderado)
                sscanf(conteudo, "%d:%d%n", &adj, &peso, &lido);
            else
                sscanf(conteudo, "%d%n", &adj, &lido);

            if (lido == 0)
                break;

            conteudo += lido;

            if (adj == v)
                continue;

            if (!existe_aresta(grafo, v, adj))
                add_aresta(grafo, v, adj, peso);
        }

        // consome o \n
        if (*conteudo == '\n')
            conteudo++;
    }
}

void importar_grafo_web(const char *conteudo, int *v_selecionado, int *a_selecionado)
{

    if (grafo_global)
    {
        destroy_grafo(grafo_global);
    }

    // Aloca e inicializa um novo grafo limpo
    grafo_global = (Grafo *)malloc(sizeof(Grafo));
    // Importante: criar_grafo já faz os mallocs internos de vértices e arestas
    criar_grafo(grafo_global, 0, 0);

    importar_lista_adj_str(grafo_global, conteudo);
}

void unificar_arestas(Grafo *grafo)
{

    for (size_t i = 0; i < grafo->quant_a; i++)
    {
        for (size_t j = 0; j < grafo->quant_a; j++)
        {
            if (i != j && grafo->arestas[i].vertice[0] == grafo->arestas[j].vertice[1] && grafo->arestas[i].vertice[1] == grafo->arestas[j].vertice[0])
            {
                excluir_aresta(grafo, j);
                break;
            }
        }
    }
}

void limpar_selecao(int *v_selecionados, int *a_selecionado)
{
    v_selecionados[0] = -1;
    v_selecionados[1] = -1;

    (*a_selecionado) = -1;
}

void mudar_cor_grafo(Grafo *grafo, Color cor, int *v_selec)
{
    if (v_selec[0] != -1)
        grafo->vertices[v_selec[0]].v.color = cor;

    if (v_selec[1] != -1)
        grafo->vertices[v_selec[1]].v.color = cor;

    if (v_selec[0] == -1 && v_selec[1] == -1)
    {
        for (size_t i = 0; i < grafo->quant_v; i++)
        {
            grafo->vertices[i].v.color = cor;
        }
    }
}

int main()
{
    int screen_w = 1280;
    int screen_h = 720;

    grafo_global = (Grafo *)malloc(sizeof(Grafo));
    criar_grafo(grafo_global, 0, 0);
    BFSAnim bfs_anim;
    bfs_anim.ativa = false;
    DFSAnim dfs_anim;
    dfs_anim.ativa = false;

    bool moving_vertex = false;
    bool moving_cam = false;
    bool exibir_hud = true;
    int vertice_movendo_atual = 0;
    int vertices_selecionados[2] = {-1, -1};
    int aresta_selecionada = -1;
    Color cor_fundo = RAYWHITE;
    Color cor_fonte = BLACK;
    Color cor = {0, 0, 0, 255};

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_w, screen_h, "Grafo");
    SetTargetFPS(60);

    Camera2D camera = {
        (Vector2){.0f, .0f},
        (Vector2){.0f, .0f},
        0,
        1,
    };

    MenuEdicao *menu_edicao_aresta = criar_menu_edicao("Editar Peso", "Peso:", (Rectangle){screen_w - 470, 360, 200, 100}, 5);
    MenuEdicao *menu_criacao_k_completo = criar_menu_edicao("Criar K Completo", "Quantidade de vértices:", (Rectangle){screen_w - 470, 460, 200, 100}, 3);
    MenuBotoes *menu_botoes = criar_menu_btn((Rectangle){GetScreenWidth() - 450, 80, 400, GetScreenHeight()});
    SandwichMenu btn_abrir_menu = criar_menu_sanduiche("docs/assets/menu_icon.png", (Vector2){GetScreenWidth() - 50, 50}, 20);
    ThemeButton btn_mudar_tema = criar_menu_tema((Circle){GetScreenWidth() - 100, 50, 20, (Color){50, 60, 90, 255}}, (Texture2D){0}, (Texture2D){0});
    MenuRGB menu_rgb_vertice = criar_menu_rgb((Rectangle){GetScreenWidth() / 2, GetScreenHeight() / 2, 650, 150});

    Texture2D ze = LoadTexture("docs/assets/ze_do_grafo.png");
    btn_mudar_tema.light = LoadTexture("docs/assets/light_mode.png");
    btn_mudar_tema.dark = LoadTexture("docs/assets/dark_mode.png");
    Font font = LoadFont("docs/fonts/Oswald.ttf");

    add_button_menu(menu_botoes, RED, "Criar Vertice");
    add_button_menu(menu_botoes, RED, "Criar Aresta");
    add_button_menu(menu_botoes, RED, "Excluir");
    add_button_menu(menu_botoes, RED, "Editar Peso");
    add_button_menu(menu_botoes, RED, "BFS");
    add_button_menu(menu_botoes, RED, "DFS");
    add_button_menu(menu_botoes, RED, "Limpar Animação");
    add_button_menu(menu_botoes, RED, "Criar K Completo");
    add_button_menu(menu_botoes, RED, "Excluir Grafo");
    add_button_menu(menu_botoes, RED, "Trocar Para Não Ponderado");
    add_button_menu(menu_botoes, RED, "Trocar Para Não Direcionado");
    add_button_menu(menu_botoes, RED, "Importar Grafo");
    add_button_menu(menu_botoes, RED, "Exportar Grafo");
    add_button_menu(menu_botoes, RED, "Editar Cor");

    gerar_k_completo(grafo_global, 10, 500.0f);

    while (!WindowShouldClose())
    {
        Vector2 mousepoint = GetMousePosition();
        Vector2 mouseWorldPos = GetScreenToWorld2D(mousepoint, camera);

        if (moving_cam == false)
            check_collision_vertex(grafo_global, &moving_vertex, mouseWorldPos, &vertice_movendo_atual);

        if (IsWindowResized())
        {
            menu_botoes->rect.x = GetScreenWidth() - 450;
            btn_mudar_tema.circle.x = GetScreenWidth() - 100;
            menu_botoes->rect.height = GetScreenHeight();
            btn_abrir_menu.x = GetScreenWidth() - 50;

            for (size_t i = 0; i < menu_botoes->quant_btn; i++)
                menu_botoes->list_btn[i].rect.x = GetScreenWidth() - 400;
        }

        if (!((CheckCollisionPointRec(mousepoint, menu_botoes->rect) && menu_botoes->aberto) || (CheckCollisionPointRec(mousepoint, menu_rgb_vertice.rect_menu) && menu_rgb_vertice.aberto)))
        {

            set_target_camera(&camera, moving_vertex, &moving_cam, mousepoint, menu_botoes->list_btn, menu_botoes->quant_btn);
            set_zoom(&camera, mousepoint, mouseWorldPos);
        }

        if (!moving_cam)
        {
            int aresta_anterior = aresta_selecionada;

            if (select_vertex(grafo_global, mouseWorldPos, vertices_selecionados))
            {
                if (aresta_selecionada != -1)
                {
                    grafo_global->arestas[aresta_selecionada].cor = BLACK;
                    aresta_selecionada = -1;
                }
            }

            if (select_aresta(grafo_global, mouseWorldPos, &aresta_selecionada) == 1)
            {
                if (aresta_anterior != -1)
                    grafo_global->arestas[aresta_anterior].cor = BLACK;
                grafo_global->arestas[aresta_selecionada].cor = RED;

                if (vertices_selecionados[0] != -1)
                    vertices_selecionados[0] = -1;
                if (vertices_selecionados[1] != -1)
                    vertices_selecionados[1] = -1;
            }
        }

        if (!moving_cam) // Add vertice
        {
            Vector2 local_criacao;
            if (onButtonClickScroll(&menu_botoes->list_btn[0], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto)
            {
                local_criacao = GetWorldToScreen2D((Vector2){0, 0}, camera);

                limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
                add_vertice(grafo_global, local_criacao.x, local_criacao.y, GREEN);
            }

            if (IsKeyPressed(KEY_V))
            {
                limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
                add_vertice(grafo_global, mouseWorldPos.x, mouseWorldPos.y, GREEN);
            }
        }

        if (((onButtonClickScroll(&menu_botoes->list_btn[1], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto) || IsKeyPressed(KEY_A)) && !moving_cam) // Add aresta
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            add_aresta_selec(grafo_global, vertices_selecionados);
            limpar_selecao(vertices_selecionados, &aresta_selecionada);
        }

        if (((onButtonClickScroll(&menu_botoes->list_btn[2], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto) || IsKeyPressed(KEY_DELETE)) && !moving_cam) // Excluir vertice ou aresta
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            if (aresta_selecionada != -1)
            {
                excluir_aresta(grafo_global, aresta_selecionada);
                aresta_selecionada = -1;
            }

            if (vertices_selecionados[0] != -1)
            {
                excluir_vertice(grafo_global, vertices_selecionados[0]);
                limpar_selecao(vertices_selecionados, &aresta_selecionada);
            }
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[3], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto && aresta_selecionada != -1 && !moving_cam) // Editar peso
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            menu_criacao_k_completo->ativa = false;
            menu_edicao_aresta->ativa = true;
            menu_edicao_aresta->rect_menu.x = GetScreenWidth() - 670;
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[4], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto && vertices_selecionados[0] != -1 && !moving_cam) // bfs
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);

            iniciar_bfs(grafo_global, &bfs_anim, vertices_selecionados[0]);
            limpar_selecao(vertices_selecionados, &aresta_selecionada);
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[5], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto && vertices_selecionados[0] != -1 && !moving_cam) // dfs
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);

            iniciar_dfs(grafo_global, &dfs_anim, vertices_selecionados[0]);
            limpar_selecao(vertices_selecionados, &aresta_selecionada);
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[6], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto && !moving_cam) // limpar animacao
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[7], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto && !moving_cam) // gerar k completo
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            menu_criacao_k_completo->ativa = true;
            menu_edicao_aresta->ativa = false;
            menu_criacao_k_completo->rect_menu.x = GetScreenWidth() - 850;
        }

        if (((onButtonClickScroll(&menu_botoes->list_btn[8], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto) || IsKeyPressedRepeat(KEY_DELETE)) && !moving_cam) // excluir grafo_global
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            int ponderado = grafo_global->ponderado;
            int direcionado = grafo_global->direcionado;
            destroy_grafo(grafo_global);
            grafo_global = (Grafo *)malloc(sizeof(Grafo));
            criar_grafo(grafo_global, direcionado, ponderado);
        }

        if (((onButtonClickScroll(&menu_botoes->list_btn[9], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto) || IsKeyPressed(KEY_P)) && !moving_cam) // excluir grafo
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            grafo_global->ponderado = !grafo_global->ponderado;

            if (grafo_global->ponderado)
                strcpy(menu_botoes->list_btn[9].text, "Trocar Para Não Ponderado");
            else
                strcpy(menu_botoes->list_btn[9].text, "Trocar Para Ponderado");
        }

        if (((onButtonClickScroll(&menu_botoes->list_btn[10], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && menu_botoes->aberto) || IsKeyPressed(KEY_O)) && !moving_cam) // excluir grafo
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            unificar_arestas(grafo_global);
            grafo_global->direcionado = !grafo_global->direcionado;

            if (grafo_global->direcionado)
                strcpy(menu_botoes->list_btn[10].text, "Trocar Para Não Direcionado");
            else
                strcpy(menu_botoes->list_btn[10].text, "Trocar Para Direcionado");
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[11], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && !moving_cam && menu_botoes->aberto) // excluir grafo
        {
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            limpar_selecao(vertices_selecionados, &aresta_selecionada);

            EM_ASM(
                abrirSeletorArquivo(););
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[12], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && !moving_cam && menu_botoes->aberto) // excluir grafo
        {
            limpar_selecao(vertices_selecionados, &aresta_selecionada);
            limpar_animacao(grafo_global, &bfs_anim, &dfs_anim);
            exportar_lista_adj(grafo_global);
        }

        if (onButtonClickScroll(&menu_botoes->list_btn[13], mousepoint, menu_botoes->scrollY, menu_botoes->rect) && !moving_cam && menu_botoes->aberto) // excluir grafo
        {
            menu_rgb_vertice.aberto = true;
        }

        if (IsKeyPressed(KEY_HOME))
        {
            camera.target = (Vector2){0, 0};
            camera.zoom = 0.5f;
        }

        if (menu_criacao_k_completo->ativa)
        {
            ler_entrada_menu(&menu_criacao_k_completo->input);

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
            {
                int n = atoi(menu_criacao_k_completo->input.buffer);

                destroy_grafo(grafo_global);
                grafo_global = (Grafo *)malloc(sizeof(Grafo));
                criar_grafo(grafo_global, 0, 0);
                gerar_k_completo(grafo_global, n, 50.f * n);

                menu_criacao_k_completo->ativa = false;
                menu_criacao_k_completo->input.char_inserted = 0;

                for (size_t i = 0; i < menu_criacao_k_completo->input.limit_char; i++)
                    menu_criacao_k_completo->input.buffer[i] = 0;
            }
        }

        if (menu_edicao_aresta->ativa)
        {

            ler_entrada_menu(&menu_edicao_aresta->input);

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
            {
                if (aresta_selecionada != -1)
                {
                    int peso = atoi(menu_edicao_aresta->input.buffer);
                    grafo_global->arestas[aresta_selecionada].peso = peso;
                    grafo_global->arestas[aresta_selecionada].cor = BLACK;
                    aresta_selecionada = -1;
                }

                menu_edicao_aresta->ativa = false;
                menu_edicao_aresta->input.char_inserted = 0;
                for (size_t i = 0; i < menu_edicao_aresta->input.limit_char; i++)
                    menu_edicao_aresta->input.buffer[i] = 0;
            }
        }

        if (menu_rgb_vertice.aberto)
        {
            switch (menu_rgb_vertice.selecao_input_atual)
            {
            case 0:
                ler_entrada_menu(&menu_rgb_vertice.r);

                int val_r = atoi(menu_rgb_vertice.r.buffer);
                if (val_r > 255)
                    val_r = 255;
                cor.r = val_r;

                break;
            case 1:
                ler_entrada_menu(&menu_rgb_vertice.g);

                int val_g = atoi(menu_rgb_vertice.g.buffer);
                if (val_g > 255)
                    val_g = 255;
                cor.g = val_g;

                break;
            case 2:
                ler_entrada_menu(&menu_rgb_vertice.b);

                int val_b = atoi(menu_rgb_vertice.b.buffer);
                if (val_b > 255)
                    val_b = 255;
                cor.b = val_b;

                break;
            case 3:
                ler_entrada_menu(&menu_rgb_vertice.a);

                int val_a = atoi(menu_rgb_vertice.a.buffer);
                if (val_a > 255)
                    val_a = 255;
                cor.a = val_a;

                break;
            }

            if (onButtonClick(&menu_rgb_vertice.confirm, mousepoint))
                mudar_cor_grafo(grafo_global, cor, vertices_selecionados);
        }

        if (IsKeyPressed(KEY_H))
            exibir_hud = !exibir_hud;

        if (!exibir_hud)
        {
            menu_botoes->aberto = false;
        }

        fechar_menu(menu_criacao_k_completo, mousepoint);
        fechar_menu(menu_edicao_aresta, mousepoint);
        fechar_menu_rgb(&menu_rgb_vertice, mousepoint);
        scroll_menu_botoes(menu_botoes);

        if (CheckCollisionPointCircle(mousepoint, (Vector2){btn_abrir_menu.x, btn_abrir_menu.y}, btn_abrir_menu.radius))
        {
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                menu_botoes->aberto = !menu_botoes->aberto; // Abre ou fecha o menu
            }
        }

        if (CheckCollisionPointCircle(mousepoint, (Vector2){btn_mudar_tema.circle.x, btn_mudar_tema.circle.y}, btn_abrir_menu.radius))
        {
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                btn_mudar_tema.theme = !btn_mudar_tema.theme; // Abre ou fecha o menu

                if (btn_mudar_tema.theme)
                    cor_fundo = (Color){30, 40, 70, 255};
                else
                    cor_fundo = RAYWHITE;
            }
        }

        if (menu_rgb_vertice.aberto && !moving_cam)
        {
            if (CheckCollisionPointRec(mousepoint, menu_rgb_vertice.rect_r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                menu_rgb_vertice.selecao_input_atual = 0;

            if (CheckCollisionPointRec(mousepoint, menu_rgb_vertice.rect_g) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                menu_rgb_vertice.selecao_input_atual = 1;

            if (CheckCollisionPointRec(mousepoint, menu_rgb_vertice.rect_b) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                menu_rgb_vertice.selecao_input_atual = 2;

            if (CheckCollisionPointRec(mousepoint, menu_rgb_vertice.rect_a) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                menu_rgb_vertice.selecao_input_atual = 3;

            if (IsKeyPressed(KEY_TAB))
            {
                menu_rgb_vertice.selecao_input_atual++;
                if (menu_rgb_vertice.selecao_input_atual > 3)
                    menu_rgb_vertice.selecao_input_atual = 0;
            }
        }

        BeginDrawing();
        ClearBackground(cor_fundo);
        BeginMode2D(camera);

        DrawTexture(ze, 100, 50000, WHITE);

        desenha_grafo(grafo_global, font, camera.zoom, btn_mudar_tema.theme);
        desenha_selecao_vertice(grafo_global, vertices_selecionados);
        update_bfs(grafo_global, &bfs_anim);
        update_dfs(grafo_global, &dfs_anim);

        EndMode2D();

        if (exibir_hud)
        {
            desenhar_menu_sanduiche(btn_abrir_menu);
            if (menu_botoes->aberto)
                desenha_menu_botoes(menu_botoes);

            if (menu_edicao_aresta->ativa)
            {
                desenha_menu_edicao(menu_edicao_aresta);
                DrawText(menu_edicao_aresta->input.buffer, menu_edicao_aresta->rect_menu.x + 90, menu_edicao_aresta->rect_menu.y + 45, 20, BLACK);
            }

            if (menu_criacao_k_completo->ativa)
            {
                desenha_menu_edicao(menu_criacao_k_completo);
                DrawText(menu_criacao_k_completo->input.buffer, menu_criacao_k_completo->rect_menu.x + menu_criacao_k_completo->rect_menu.width - 120, menu_criacao_k_completo->rect_menu.y + 45, 20, BLACK);
            }

            desenha_theme_button(btn_mudar_tema);

            if (menu_rgb_vertice.aberto)
            {
                desenhar_menu_rgb(menu_rgb_vertice);
                DrawText(menu_rgb_vertice.r.buffer, menu_rgb_vertice.rect_r.x + 10, menu_rgb_vertice.rect_r.y + 10, 20, BLACK);
                DrawText(menu_rgb_vertice.g.buffer, menu_rgb_vertice.rect_g.x + 10, menu_rgb_vertice.rect_g.y + 10, 20, BLACK);
                DrawText(menu_rgb_vertice.b.buffer, menu_rgb_vertice.rect_b.x + 10, menu_rgb_vertice.rect_b.y + 10, 20, BLACK);
                DrawText(menu_rgb_vertice.a.buffer, menu_rgb_vertice.rect_a.x + 10, menu_rgb_vertice.rect_a.y + 10, 20, BLACK);
                DrawRectangleRounded((Rectangle){menu_rgb_vertice.rect_menu.x + menu_rgb_vertice.rect_menu.width - 50, menu_rgb_vertice.rect_menu.y + 25, 35, 35}, 0.2f, 3, cor);
            }

            if (btn_mudar_tema.theme)
                cor_fonte = WHITE;

            DrawText(TextFormat("Vertices: %d", grafo_global->quant_v), 10, 10, 20, cor_fonte);
            DrawText(TextFormat("Arestas: %d", grafo_global->quant_a), 10, 40, 20, cor_fonte);
            DrawText(TextFormat("FPS: %d", GetFPS()), 10, 70, 20, cor_fonte);
            DrawText(TextFormat("Grafo %s %s", grafo_global->direcionado ? "Orientado" : "Não Orientado", grafo_global->ponderado ? "Ponderado" : "Não Ponderado"), 10, 100, 20, cor_fonte);
        }

        EndDrawing();
    };

    UnloadTexture(ze);
    UnloadTexture(btn_abrir_menu.img);
    UnloadTexture(btn_mudar_tema.dark);
    UnloadTexture(btn_mudar_tema.light);

    CloseWindow();
    free(menu_criacao_k_completo);
    free(menu_edicao_aresta);

    for (size_t i = 0; i < menu_botoes->quant_btn; i++)
    {
        free(menu_botoes->list_btn[i].text);
    }

    free(menu_botoes);

    destroy_grafo(grafo_global);
}