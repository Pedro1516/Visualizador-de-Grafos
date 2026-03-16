#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>

typedef struct
{
    float x;
    float y;
    float radius;
    Color color;
} Circle;

typedef struct
{
    Rectangle rect;
    const char *text;
    Color color;
    bool sombra;
    bool clicked;
} Button;

typedef struct
{
    char *buffer;
    int char_inserted;
    int limit_char;
} InputText;

typedef struct
{
    char *titulo;
    char *label;
    Rectangle rect_menu;
    InputText input;
    bool ativa;
} MenuEdicao;

bool onButtonClick(Button *button, Vector2 mousepoint);

void drawButton(Button *button, int font_size);

Button *create_button_rect(Rectangle rect, Color color, const char *text);

void desenha_menu_edicao(MenuEdicao *menu);

MenuEdicao *criar_menu_edicao(char *titulo, char *label, Rectangle rect_menu, int limit_char);