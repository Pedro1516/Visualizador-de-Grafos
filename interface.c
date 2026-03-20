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

void drawButtonRec(Button *button, Rectangle rect, int font_size)
{
    if (button->sombra)
    {
        font_size = font_size * rect.height * 0.02;
        int text_size = MeasureText(button->text, font_size);

        Rectangle sombra = rect;
        sombra.height += sombra.height * 0.2;

        DrawRectangleRounded(sombra, 0.5, 0, (Color){100, 0, 0, 255});
        DrawRectangleRounded(rect, 0.5, 0, button->color);

        DrawText(button->text,
                 rect.x + rect.width / 2 - text_size / 2,
                 rect.y + rect.height / 2 - font_size / 2,
                 font_size, BLACK);
    }
    else
    {
        font_size = font_size * rect.height * 0.02;
        int text_size = MeasureText(button->text, font_size);

        Rectangle aux = rect;
        aux.y += rect.height * 0.2;

        DrawRectangleRounded(aux, 0.5, 0, button->color);

        DrawText(button->text,
                 aux.x + aux.width / 2 - text_size / 2,
                 aux.y + aux.height / 2 - font_size / 2,
                 font_size, BLACK);
    }
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

Button create_button_rect(Rectangle rect, Color color, const char *text)
{
    Button button;
    button.color = color;
    button.rect = rect;
    button.text = strdup(text);
    button.sombra = true;
    button.clicked = false;
    return button;
}

MenuEdicao *criar_menu_edicao(char *titulo, char *label, Rectangle rect_menu, int limit_char)
{
    MenuEdicao *menu = (MenuEdicao *)malloc(sizeof(MenuEdicao));

    menu->titulo = strdup(titulo);
    menu->label = strdup(label);

    menu->rect_menu = rect_menu;
    menu->ativa = false;

    menu->input.buffer = (char *)calloc(limit_char, sizeof(char));
    menu->input.limit_char = limit_char;
    menu->input.char_inserted = 0;

    int tam_text_titulo = MeasureText(menu->titulo, 20);
    int tam_text_label = MeasureText(menu->label, 20);
    int tam_text = (tam_text_titulo > tam_text_label) ? tam_text_titulo : tam_text_label;

    if (tam_text > menu->rect_menu.width)
        menu->rect_menu.width = tam_text + 150;

    return menu;
}

void desenha_menu_edicao(MenuEdicao *menu)
{
    DrawRectangleRec(menu->rect_menu, (Color){50, 60, 90, 255});
    DrawRectangleLines(menu->rect_menu.x, menu->rect_menu.y, menu->rect_menu.width, menu->rect_menu.height, (Color){100, 100, 100, 255});
    DrawText(menu->titulo, menu->rect_menu.x + 10, menu->rect_menu.y + 10, 20, WHITE);
    DrawText(menu->label, menu->rect_menu.x + 10, menu->rect_menu.y + 50, 20, WHITE);

    int tam_label = MeasureText(menu->label, 20);
    DrawRectangle(menu->rect_menu.x + tam_label + 20, menu->rect_menu.y + 40, 100, 30, WHITE);
}

void desenha_menu_botoes(MenuBotoes *menu)
{
    DrawRectangleRec(menu->rect, (Color){50, 60, 90, 255});
    DrawRectangleLines(menu->rect.x, menu->rect.y, menu->rect.width, menu->rect.height, (Color){100, 100, 100, 255});

    BeginScissorMode(menu->rect.x, menu->rect.y, menu->rect.width, menu->rect.height);

    for (int i = 0; i < menu->quant_btn; i++)
    {
        Rectangle r = menu->list_btn[i].rect;
        r.y += menu->scrollY;

        drawButtonRec(&menu->list_btn[i], r, 20);
    }

    EndScissorMode();
}

MenuBotoes *criar_menu_btn(Rectangle rect)
{
    MenuBotoes *menu = (MenuBotoes *)malloc(sizeof(MenuBotoes));
    menu->list_btn = (Button *)malloc(sizeof(Button) * 10);
    menu->limit_btn = 10;
    menu->rect = rect;
    menu->quant_btn = 0;
    menu->scrollY = 0;
    menu->aberto = 0;

    return menu;
}

void add_button_menu(MenuBotoes *menu, Color color, char *label)
{
    if (menu->quant_btn == menu->limit_btn)
    {
        menu->limit_btn *= 2;
        menu->list_btn = (Button *)realloc(menu->list_btn, sizeof(Button) * menu->limit_btn);
    }

    menu->list_btn[menu->quant_btn] = create_button_rect((Rectangle){GetScreenWidth() - 250, menu->rect.y + 20 + 100 * menu->quant_btn, 320, 50}, color, label);
    menu->quant_btn++;
}

void scroll_menu_botoes(MenuBotoes *menu)
{
    Vector2 mouse = GetMousePosition();

    // só ativa scroll dentro do menu
    if (CheckCollisionPointRec(mouse, menu->rect))
    {
        float wheel = GetMouseWheelMove();
        menu->scrollY += wheel * 30; // velocidade do scroll
    }

    // ===== calcular altura total dos botões =====
    float altura_total = menu->quant_btn * 100 + 80; // porque você usa 100 no add_button_menu

    // ===== limites =====

    // não deixa puxar pra baixo
    if (menu->scrollY > 0)
        menu->scrollY = 0;

    // não deixa subir demais
    float limite_inferior = menu->rect.height - altura_total;

    if (limite_inferior > 0)
        limite_inferior = 0;

    if (menu->scrollY < limite_inferior)
        menu->scrollY = limite_inferior;
}

bool onButtonClickScroll(Button *button, Vector2 mousepoint, float scrollY, Rectangle menuRect)
{
    Rectangle r = button->rect;
    r.y += scrollY;

    button->clicked = false;

    // só interage dentro do menu
    if (!CheckCollisionPointRec(mousepoint, menuRect))
        return false;

    bool collision = CheckCollisionPointRec(mousepoint, r);

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

SandwichMenu criar_menu_sanduiche(char *icon, Vector2 pos, int radius)
{
    SandwichMenu menu;
    menu.img = LoadTexture(icon);
    menu.radius = radius;
    menu.x = pos.x;
    menu.y = pos.y;

    return menu;
}

void desenhar_menu_sanduiche(SandwichMenu menu)
{
    DrawCircle(menu.x, menu.y, menu.radius, (Color){50, 60, 90, 255});
    DrawTexture(menu.img, menu.x - menu.img.width / 2, menu.y - menu.img.height / 2, WHITE);
}
