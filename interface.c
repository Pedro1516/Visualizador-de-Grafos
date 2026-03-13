#include "interface.h"

bool onButtonClick(Button *button, Vector2 mousepoint)
{
    bool collision;
    button->clicked = false;

    collision = CheckCollisionPointRec(mousepoint, button->rect);

    if (collision)
    {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            button->clicked = true;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            button->sombra = false;
        else
            button->sombra = true;
    }
    else
    {
        button->sombra = true;
    }

    return button->clicked;
}

void drawButton(Button *button, int font_size)
{
    if (button->sombra)
    {
        font_size = font_size * button->rect.height * 0.02;
        int text_size = MeasureText(button->text, font_size);

        // Desenha sombra
        Rectangle sombra = button->rect;
        sombra.height += sombra.height * 0.2;
        DrawRectangleRounded(sombra, 0.5, 0, (Color){100, 0, 0, 255});
        // Desenha botão
        DrawRectangleRounded(button->rect, 0.5, 0, button->color);
        DrawText(button->text, button->rect.x + button->rect.width / 2 - text_size / 2, button->rect.y + button->rect.height / 2 - font_size / 2, font_size, BLACK);
    }
    else
    {
        font_size = font_size * button->rect.height * 0.02;
        int text_size = MeasureText(button->text, font_size);
        Rectangle aux = button->rect;
        aux.y += button->rect.height * 0.2;
        // Desenha botão
        DrawRectangleRounded(aux, 0.5, 0, button->color);
        DrawText(button->text, aux.x + aux.width / 2 - text_size / 2, aux.y + aux.height / 2 - font_size / 2, font_size, BLACK);
    }
}

Button *create_button_rect(Rectangle rect, Color color, const char *text)
{
    Button *button = (Button *)malloc(sizeof(Button));
    button->color = color;
    button->rect = rect;
    button->text = text;
    button->sombra = true;
    button->clicked = false;
    return button;
}

void desenha_menu_edicao_aresta(MenuEdicaoAresta *menu){
    DrawRectangleRec(menu->rect_menu, (Color){50, 60, 90, 255});
    DrawRectangleLines(menu->rect_menu.x, menu->rect_menu.y, menu->rect_menu.width, menu->rect_menu.height, (Color){100, 100, 100, 255});
    DrawText("Editar Aresta", menu->rect_menu.x + 10, menu->rect_menu.y + 10, 20, WHITE);
    DrawText("Peso:", menu->rect_menu.x + 10, menu->rect_menu.y + 50, 20, WHITE);
    DrawRectangle(menu->rect_menu.x + 80, menu->rect_menu.y + 40, 100, 30, WHITE);
}