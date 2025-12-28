#pragma once 
#include <raylib.h>
#include <raymath.h>
#include <iostream>

namespace TEST{

/*
    Por enquanto está aparecendo apenas no último element, mas depois
    podemos criar um padrão singleton, em que o controler edita todos os componentes,
    assim também evita overhead na execução do código quando não se está em EditMode!
*/

constexpr int TOPLEFT = 1;
constexpr int TOPRIGHT = 2;
constexpr int BOTTOMLEFT = 3;
constexpr int BOTTOMRIGHT = 4;
constexpr int TOP = 1;
constexpr int BOTTOM = 2;
constexpr int LEFT = 3;
constexpr int RIGHT = 4;

/* 
    DEPOIS SEPARAR ENTRE O ESTADO DE RESIZE/MOVE
    E O ESTADO DE HOVER EM QUE SIMPLESMENTE DETECTA-SE SE O MOUSE ESTÁ EM CIMA
    SÃO ESTADOS COM COMPORTAMENTOS DIFERENTES
*/
enum EditState{
    MOVE_HOVER_STATE,
    RESIZE_DIAGONAL_HOVER_STATE,
    RESIZE_HOVER_STATE,
    MOVE_STATE,
    RESIZE_DIAGONAL_STATE,
    RESIZE_STATE,
    IDLE_STATE
};

inline int to_positive(int x) { return x < 0 ? -x : x; }

// depois separar este em outro arquivo
void DrawDashedRectangle(Rectangle rec, int dashLength, int gapLength, Color color) {
    int xEnd = rec.x + rec.width;
    int yEnd = rec.y + rec.height;

    // Top side
    for (int x = rec.x; x < xEnd; x += dashLength + gapLength) {
        int segmentEnd = (x + dashLength > xEnd) ? xEnd : x + dashLength;
        DrawLine(x, rec.y, segmentEnd, rec.y, color);
    }

    // Right side
    for (int y = rec.y; y < yEnd; y += dashLength + gapLength) {
        int segmentEnd = (y + dashLength > yEnd) ? yEnd : y + dashLength;
        DrawLine(xEnd, y, xEnd, segmentEnd, color);
    }

    // Bottom side
    for (int x = xEnd; x > rec.x; x -= dashLength + gapLength) {
        int segmentStart = (x - dashLength < rec.x) ? rec.x : x - dashLength;
        DrawLine(x, yEnd, segmentStart, yEnd, color);
    }

    // Left side
    for (int y = yEnd; y > rec.y; y -= dashLength + gapLength) {
        int segmentStart = (y - dashLength < rec.y) ? rec.y : y - dashLength;
        DrawLine(rec.x, y, rec.x, segmentStart, color);
    }
}

inline int CheckPointOnCornerRec(Vector2 point, const Rectangle& rec, int tol_radius = 4)
{
    Vector2 topleft, topright, bottomleft, bottomright;

    topleft = {rec.x,rec.y};
    topright = {rec.x+rec.width,rec.y};
    bottomleft = {rec.x,rec.y+rec.height};
    bottomright = {rec.x+rec.width,rec.y+rec.height};

    float tol = static_cast<float>(tol_radius);

    int i = 1;
    for(auto border : {topleft, topright, bottomleft, bottomright}){
        if(Vector2Distance(border,point) <= tol) 
            return i;
        else i++;
    }
    
    return 0;
}

inline int CheckPointOnBorderRec(Vector2 point, const Rectangle& rec, int tol_radius = 4)
{

    int x_dif = point.x - rec.x;
    int y_dif = point.y - rec.y;

    bool is_on_x = point.x >= rec.x && point.x <= (rec.x+rec.width);
    bool is_on_y = point.y >= rec.y && point.y <= (rec.y+rec.height);

    bool is_y_top = to_positive(y_dif) <= tol_radius;
    bool is_y_bottom = to_positive(y_dif - rec.height) <= tol_radius;
    bool is_x_left = to_positive(x_dif) <= tol_radius;
    bool is_x_right = to_positive(x_dif - rec.width) <= tol_radius;


    if(is_y_top && is_on_x) return TOP;
    if(is_y_bottom && is_on_x) return BOTTOM;
    if(is_x_left && is_on_y) return LEFT;
    if(is_x_right && is_on_y) return RIGHT;

    return 0;
}

inline void process_move(Vector2 delta, Rectangle& rec)
{
    rec.x += delta.x;
    rec.y += delta.y;
}

inline void process_resize_diagonal(int border_selected, Vector2 delta, Rectangle& rec)
{
    if(border_selected == TOPLEFT)
    {
        rec.width -= delta.x;
        rec.height -= delta.y;
        rec.x += delta.x;
        rec.y += delta.y;
    }

    if(border_selected == TOPRIGHT)
    {
        rec.width += delta.x;
        rec.height -= delta.y;
        rec.y += delta.y;
    }

    if(border_selected == BOTTOMLEFT)
    {
        rec.width -= delta.x;
        rec.height += delta.y;
        rec.x += delta.x;
    }

    if(border_selected == BOTTOMRIGHT)
    {
        rec.width += delta.x;
        rec.height += delta.y;
    }
}

inline void process_resize(int border_selected, Vector2 delta, Rectangle& rec)
{
    if(border_selected == TOP)
    {
        rec.height -= delta.y;
        rec.y += delta.y;
    }

    if(border_selected == RIGHT)
    {
        rec.width += delta.x;
    }

    if(border_selected == BOTTOM)
    {
        rec.height += delta.y;
    }

    if(border_selected == LEFT)
    {
        rec.width -= delta.x;
        rec.x += delta.x;
    }
}

class EditRectangleControler
{
private:
    EditState state;
    Rectangle* rec;
    Vector2* position;
    int corner_selected{0};
    int border_selected{0};
public:
    EditRectangleControler(Rectangle* rec_)
        : rec(rec_){}
    
    void EditPosition()
    {
        Vector2 mouse = GetMousePosition();

        if(state != IDLE_STATE)
            DrawDashedRectangle(*rec,20,5,RED);

        int corner = 0;
        int border = 0;
        /* Resize */
        if((corner = CheckPointOnCornerRec(mouse,*rec)) && !(state == RESIZE_STATE) && !(state == MOVE_STATE))
        {
            if(state != RESIZE_DIAGONAL_HOVER_STATE)
            {
                state = RESIZE_DIAGONAL_HOVER_STATE;
                corner_selected = corner;
            }
            if(corner == TOPRIGHT || corner == BOTTOMLEFT) SetMouseCursor(MOUSE_CURSOR_RESIZE_NESW);
            if(corner == TOPLEFT || corner == BOTTOMRIGHT) SetMouseCursor(MOUSE_CURSOR_RESIZE_NWSE);
        }
        else if((border = CheckPointOnBorderRec(mouse,*rec)) && !(state == RESIZE_DIAGONAL_STATE) && !(state == MOVE_STATE))
        {
            if(state != RESIZE_HOVER_STATE)
            {
                state = RESIZE_HOVER_STATE;
                border_selected = border;
            }
            if(border == TOP || border == BOTTOM) SetMouseCursor(MOUSE_CURSOR_RESIZE_NS);
            if(border == LEFT || border == RIGHT) SetMouseCursor(MOUSE_CURSOR_RESIZE_EW);
        }
        else if (CheckCollisionPointRec(mouse, *rec) && !(state == RESIZE_DIAGONAL_STATE) && !(state == RESIZE_STATE)) {
            state = MOVE_HOVER_STATE;
            SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
        }else if(state != MOVE_STATE && state != RESIZE_DIAGONAL_STATE && state != RESIZE_STATE) {
            state = IDLE_STATE;
            SetMouseCursor(MOUSE_CURSOR_ARROW);
        }

        /* Posição */
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if(state == MOVE_HOVER_STATE)
                state = MOVE_STATE;
            if(state == RESIZE_DIAGONAL_HOVER_STATE)
                state = RESIZE_DIAGONAL_STATE;
            if(state == RESIZE_HOVER_STATE)
                state = RESIZE_STATE;

            auto delta = GetMouseDelta();

            if(state == MOVE_STATE)
                process_move(delta, *rec);
            else if(state == RESIZE_DIAGONAL_STATE)
                process_resize_diagonal(corner_selected, delta, *rec);
            else if(state == RESIZE_STATE)
                process_resize(border_selected,delta,*rec);
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = IDLE_STATE;
            SetMouseCursor(MOUSE_CURSOR_ARROW);
        }

        // debug
        if(false)
        {
            DrawText(TextFormat("%d",static_cast<int>(state)),100,100,24,BLACK);
            DrawText(TextFormat("X: %f Y: %f", mouse.x, mouse.y),20,400,24,BLACK);
            DrawText(TextFormat("Rec X: %f Rec Y: %f", (*rec).x, (*rec).y),20,500,24,BLACK);
            DrawText(TextFormat("Rec W: %f Rec H: %f", (*rec).width, (*rec).height),20,600,24,BLACK);

            const char* text = "NONE";
            if(corner_selected == TOPLEFT) text = "TOPLEFT";
            if(corner_selected == TOPRIGHT) text = "TOPRIGHT";
            if(corner_selected == BOTTOMLEFT) text = "BOTTOMLEFT";
            if(corner_selected == BOTTOMRIGHT) text = "BOTTOMRIGHT";
            DrawText(text,20,700,24,BLACK);
        }
    }
};

}