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
    char *text;
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

typedef struct
{
    Button *list_btn;
    Rectangle rect;
    int limit_btn;
    int quant_btn;
    int scrollY;
    int aberto;
} MenuBotoes;

typedef struct
{
    Texture2D img;
    int x;
    int y;
    int radius;
} SandwichMenu;

typedef struct
{
    Texture2D dark;
    Texture2D light;
    Circle circle;
    bool theme;
} ThemeButton;

typedef struct
{
    Rectangle rect_menu;
    Rectangle rect_r;
    Rectangle rect_g;
    Rectangle rect_b;
    Rectangle rect_a;
    InputText r;
    InputText g;
    InputText b;
    InputText a;
    int selecao_input_atual;
} MenuRGB;

SandwichMenu criar_menu_sanduiche(char *icon, Vector2 pos, int radius);
ThemeButton criar_menu_tema(Circle corpo, Texture2D dark, Texture2D light);
MenuEdicao *criar_menu_edicao(char *titulo, char *label, Rectangle rect_menu, int limit_char);
MenuBotoes *criar_menu_btn(Rectangle rect);
Button create_button_rect(Rectangle rect, Color color, const char *text);
MenuRGB criar_menu_rgb(Rectangle pos_menu);

void drawButtonRec(Button *button, Rectangle rect, int font_size);
void drawButton(Button *button, int font_size);
void desenha_menu_edicao(MenuEdicao *menu);
void desenha_menu_botoes(MenuBotoes *menu);
void desenha_theme_button(ThemeButton btn);
void desenhar_menu_sanduiche(SandwichMenu menu);
void desenhar_menu_rgb(MenuRGB menu);

void add_button_menu(MenuBotoes *menu, Color color, char *label);
void scroll_menu_botoes(MenuBotoes *menu);
bool onButtonClickScroll(Button *button, Vector2 mousepoint, float scrollY, Rectangle menuRect);
bool onButtonClick(Button *button, Vector2 mousepoint);