#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

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
    int height;
    int width;
    int pergunta_atual;
    int streak;
} MenuSuperior;


bool onButtonClick(Button *button, Vector2 mousepoint);

void drawButton(Button *button, int font_size);

Button *create_button_rect(Rectangle rect, Color color, const char *text);

