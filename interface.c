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
                 font_size, RAYWHITE);
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
                 font_size, RAYWHITE);
    }
}

void drawButton(Button *button, int font_size)
{
    Color cor_fonte = BLACK;
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
        DrawText(button->text, button->rect.x + button->rect.width / 2 - text_size / 2, button->rect.y + button->rect.height / 2 - font_size / 2, font_size, cor_fonte);
    }
    else
    {
        font_size = font_size * button->rect.height * 0.02;
        int text_size = MeasureText(button->text, font_size);
        Rectangle aux = button->rect;
        aux.y += button->rect.height * 0.2;
        // Desenha botão
        DrawRectangleRounded(aux, 0.5, 0, button->color);
        DrawText(button->text, aux.x + aux.width / 2 - text_size / 2, aux.y + aux.height / 2 - font_size / 2, font_size, cor_fonte);
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
    DrawRectangleRounded(menu->rect_menu, 0.1f, 2, (Color){50, 60, 90, 255});
    DrawRectangleRoundedLines(menu->rect_menu, 0.1f, 2, (Color){100, 100, 100, 255});
    DrawText(menu->titulo, menu->rect_menu.x + 10, menu->rect_menu.y + 10, 20, WHITE);
    DrawText(menu->label, menu->rect_menu.x + 10, menu->rect_menu.y + 50, 20, WHITE);

    int tam_label = MeasureText(menu->label, 20);
    DrawRectangle(menu->rect_menu.x + tam_label + 20, menu->rect_menu.y + 40, 100, 30, WHITE);
}

void desenha_menu_botoes(MenuBotoes *menu)
{
    DrawRectangleRounded(menu->rect, 0.1f, 2, (Color){50, 60, 90, 255});
    DrawRectangleRoundedLines(menu->rect, 0.1f, 2, (Color){100, 100, 100, 255});

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

void desenha_menu_sanduiche(SandwichMenu menu)
{
    DrawCircle(menu.x, menu.y, menu.radius, (Color){50, 60, 90, 255});
    DrawTexture(menu.img, menu.x - menu.img.width / 2, menu.y - menu.img.height / 2, WHITE);
}

ThemeButton criar_menu_tema(Circle corpo, Texture2D dark, Texture2D light)
{
    ThemeButton btn;
    btn.circle = corpo;
    btn.dark = dark;
    btn.light = light;
    btn.theme = 1;

    return btn;
}

void desenha_theme_button(ThemeButton btn)
{
    DrawCircle(btn.circle.x, btn.circle.y, btn.circle.radius, btn.circle.color);

    if (!btn.theme)
        DrawTexture(btn.light, btn.circle.x - btn.light.width / 2, btn.circle.y - btn.light.height / 2, WHITE);
    else
        DrawTexture(btn.dark, btn.circle.x - btn.dark.width / 2, btn.circle.y - btn.dark.height / 2, WHITE);
}

void desenha_menu_rgb(MenuRGB menu, Font font)
{
    DrawRectangleRounded(menu.rect_menu, 0.1f, 2, (Color){50, 60, 90, 255});
    DrawRectangleRoundedLines(menu.rect_menu, 0.1f, 2, (Color){100, 100, 100, 255});

    DrawTextEx(font, "R:", (Vector2){menu.rect_r.x - 30, menu.rect_r.y + 5}, 40, 0.1f, WHITE);
    DrawRectangleRec(menu.rect_r, WHITE);
    DrawText(menu.r.buffer, menu.rect_r.x + 10, menu.rect_r.y + 10, 20, BLACK);

    DrawTextEx(font, "G:", (Vector2){menu.rect_g.x - 30, menu.rect_g.y + 5}, 40, 0.1f, WHITE);
    DrawRectangleRec(menu.rect_g, WHITE);
    DrawText(menu.g.buffer, menu.rect_g.x + 10, menu.rect_g.y + 10, 20, BLACK);

    DrawTextEx(font, "B: ", (Vector2){menu.rect_b.x - 30, menu.rect_b.y + 5}, 40, 0.1f, WHITE);
    DrawRectangleRec(menu.rect_b, WHITE);
    DrawText(menu.b.buffer, menu.rect_b.x + 10, menu.rect_b.y + 10, 20, BLACK);

    DrawTextEx(font, "A: ", (Vector2){menu.rect_a.x - 30, menu.rect_a.y + 5}, 40, 0.1f, WHITE);
    DrawRectangleRec(menu.rect_a, WHITE);
    DrawText(menu.a.buffer, menu.rect_a.x + 10, menu.rect_a.y + 10, 20, BLACK);

    Rectangle rec_list[4] = {menu.rect_r, menu.rect_g, menu.rect_b, menu.rect_a};
    Rectangle rec_aux = {rec_list[menu.selecao_input_atual].x - 1, rec_list[menu.selecao_input_atual].y - 1, rec_list[menu.selecao_input_atual].width + 2, rec_list[menu.selecao_input_atual].height + 2};
    DrawRectangleLinesEx(rec_aux, 3, (Color){255, 0, 50, 255});

    desenha_roda_de_cor(menu.colorpicker);

    float nova_altura = menu.slider.height - menu.slider.height * menu.colorpicker.hsv_value;
    //  float novo_y = menu.slider.y + menu.slider.y * menu.colorpicker.hsv_value;
    float novo_y = menu.slider.y;

    DrawRectangleRec(menu.slider, (Color){200, 200, 200, 255});
    DrawRectangle(menu.slider.x, novo_y, menu.slider.width, nova_altura, (Color){100, 100, 100, 255});

    drawButton(&menu.confirm, 30);
}

MenuRGB criar_menu_rgb(Rectangle pos_menu)
{
    MenuRGB menu;
    menu.selecao_input_atual = 0; // vermelho
    menu.rect_menu = pos_menu;
    menu.aberto = false;
    menu.slider_ativo = false;

    menu.rect_r = (Rectangle){pos_menu.x + pos_menu.width - 150, pos_menu.y + 20, 100, 40};
    menu.rect_g = (Rectangle){pos_menu.x + pos_menu.width - 150, menu.rect_r.y + 60, 100, 40};
    menu.rect_b = (Rectangle){pos_menu.x + pos_menu.width - 150, menu.rect_g.y + 60, 100, 40};
    menu.rect_a = (Rectangle){pos_menu.x + pos_menu.width - 150, menu.rect_b.y + 60, 100, 40};
    menu.slider = (Rectangle){pos_menu.x + pos_menu.width - 80, menu.rect_a.y + 80, 20, 100};
    menu.confirm = create_button_rect((Rectangle){pos_menu.x + pos_menu.width - 130, pos_menu.y + pos_menu.height - 60, 100, 40}, (Color){50, 60, 131, 255}, "Aplicar");

    menu.r.buffer = (char *)calloc(4, sizeof(char));
    menu.g.buffer = (char *)calloc(4, sizeof(char));
    menu.b.buffer = (char *)calloc(4, sizeof(char));
    menu.a.buffer = (char *)calloc(4, sizeof(char));
    strcpy(menu.a.buffer, "255");

    menu.r.char_inserted = 0;
    menu.g.char_inserted = 0;
    menu.b.char_inserted = 0;
    menu.a.char_inserted = 3;

    menu.r.limit_char = 3;
    menu.g.limit_char = 3;
    menu.b.limit_char = 3;
    menu.a.limit_char = 3;

    menu.colorpicker = (ColorPicker){64, {menu.rect_menu.x + menu.rect_menu.width / 2 - 75, menu.rect_menu.y + menu.rect_menu.height / 2}, (Vector2){menu.rect_menu.x + menu.rect_menu.width / 2 - 75, menu.rect_menu.y + menu.rect_menu.height / 2}, 200.0f, false, 1.0f, (Color){255, 255, 255, 255}};

    return menu;
}

void desenha_roda_de_cor(ColorPicker roda)
{
    rlBegin(RL_TRIANGLES);
    for (unsigned int i = 0; i < roda.triangle_count; i++)
    {
        float angleOffset = ((PI * 2.0f) / (float)roda.triangle_count);
        float angle = angleOffset * (float)i;
        float angleOffsetCalculated = ((float)i + 1) * angleOffset;
        Vector2 scale = (Vector2){roda.pointScale, roda.pointScale};

        Vector2 offset = Vector2Multiply((Vector2){sinf(angle), -cosf(angle)}, scale);
        Vector2 offset2 = Vector2Multiply((Vector2){sinf(angleOffsetCalculated), -cosf(angleOffsetCalculated)}, scale);

        Vector2 position = Vector2Add(roda.center, offset);
        Vector2 position2 = Vector2Add(roda.center, offset2);

        float angleNonRadian = (angle / (2.0f * PI)) * 360.0f;
        float angleNonRadianOffset = (angleOffset / (2.0f * PI)) * 360.0f;

        Color currentColor = ColorFromHSV(angleNonRadian, 1.0f, 1.0f);
        Color offsetColor = ColorFromHSV(angleNonRadian + angleNonRadianOffset, 1.0f, 1.0f);

        // RL_TRIANGLES expects three vertices per triangle
        rlColor4ub(currentColor.r, currentColor.g, currentColor.b, currentColor.a);
        rlVertex2f(position.x, position.y);
        rlColor4f(roda.hsv_value, roda.hsv_value, roda.hsv_value, 1.0f);
        rlVertex2f(roda.center.x, roda.center.y);
        rlColor4ub(offsetColor.r, offsetColor.g, offsetColor.b, offsetColor.a);
        rlVertex2f(position2.x, position2.y);
    }
    rlEnd();

    Color handleColor = BLACK;

    if (Vector2Distance(roda.center, roda.selector) / roda.pointScale <= 0.5f && roda.hsv_value <= 0.5f)
    {
        handleColor = DARKGRAY;
    }

    // Draw the color handle
    DrawCircleLinesV(roda.selector, 5.0f, handleColor);
}

void atualiza_cor_roda(MenuRGB *menu, Vector2 mousepoint)
{

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && Vector2Distance(GetMousePosition(), menu->colorpicker.center) <= menu->colorpicker.pointScale + 10.0f && !menu->slider_ativo)
    {
        menu->colorpicker.selecao_ativa = true;
    }

    if (menu->colorpicker.selecao_ativa)
    {
        if (menu->colorpicker.selecao_ativa)
            menu->colorpicker.selector = mousepoint;

        float distance = Vector2Distance(menu->colorpicker.center, menu->colorpicker.selector) / menu->colorpicker.pointScale;

        float angle = ((Vector2Angle((Vector2){0.0f, -menu->colorpicker.pointScale}, Vector2Subtract(menu->colorpicker.center, menu->colorpicker.selector)) / PI + 1.0f) / 2.0f);
        if (menu->colorpicker.selecao_ativa && distance > 1.0f)
            menu->colorpicker.selector = Vector2Add((Vector2){sinf(angle * (PI * 2.0f)) * menu->colorpicker.pointScale, -cosf(angle * (PI * 2.0f)) * menu->colorpicker.pointScale}, menu->colorpicker.center);

        float angle360 = angle * 360.0f;
        float valueActual = Clamp(distance, 0.0f, 1.0f);
        menu->colorpicker.cor_atual = ColorLerp((Color){(int)(menu->colorpicker.hsv_value * 255.0f), (int)(menu->colorpicker.hsv_value * 255.0f), (int)(menu->colorpicker.hsv_value * 255.0f), 255}, ColorFromHSV(angle360, Clamp(distance, 0.0f, 1.0f), 1.0f), valueActual);

        snprintf(menu->r.buffer, 5, "%d", menu->colorpicker.cor_atual.r);
        menu->r.char_inserted = (atoi(menu->r.buffer) < 10) ? 1 : (atoi(menu->r.buffer) < 100) ? 2
                                                                                               : 3;

        snprintf(menu->g.buffer, 5, "%d", menu->colorpicker.cor_atual.g);
        menu->g.char_inserted = (atoi(menu->g.buffer) < 10) ? 1 : (atoi(menu->g.buffer) < 100) ? 2
                                                                                               : 3;

        snprintf(menu->b.buffer, 5, "%d", menu->colorpicker.cor_atual.b);
        menu->b.char_inserted = (atoi(menu->b.buffer) < 10) ? 1 : (atoi(menu->b.buffer) < 100) ? 2
                                                                                               : 3;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        menu->colorpicker.selecao_ativa = false;
}

void atualiza_slider_rgb(MenuRGB *menu, Vector2 mousepoint)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousepoint, menu->slider))
    {
        menu->slider_ativo = true;
    }

    if (menu->slider_ativo)
    {
        float value = (mousepoint.y - menu->slider.y) / menu->slider.height;
        value = Clamp(value, 0.0f, 1.0f);
        menu->colorpicker.hsv_value = 1.0f - value;

        int val_r = (int)(menu->colorpicker.cor_atual.r * menu->colorpicker.hsv_value);
        int val_g = (int)(menu->colorpicker.cor_atual.g * menu->colorpicker.hsv_value);
        int val_b = (int)(menu->colorpicker.cor_atual.b * menu->colorpicker.hsv_value);

        snprintf(menu->r.buffer, menu->r.limit_char + 1, "%d", val_r);
        menu->r.char_inserted = (val_r < 10) ? 1 : (val_r < 100) ? 2
                                                                 : 3;

        snprintf(menu->g.buffer, menu->g.limit_char + 1, "%d", val_g);
        menu->g.char_inserted = (val_g < 10) ? 1 : (val_g < 100) ? 2
                                                                 : 3;

        snprintf(menu->b.buffer, menu->b.limit_char + 1, "%d", val_b);
        menu->b.char_inserted = (val_b < 10) ? 1 : (val_b < 100) ? 2
                                                                 : 3;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        menu->slider_ativo = false;
}