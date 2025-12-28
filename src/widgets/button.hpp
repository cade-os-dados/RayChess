#pragma once
#include <raylib.h>
#include "hud_utils.hpp"

class Button
{
private:
    Rectangle rec;
    Font font = GetFontDefault();
    const char* texto = "";
    Color color{WHITE};
public:
    Button(Rectangle);
    Rectangle* GetRec();
    inline bool Clicked();
    void SetText(const char*);
    void SetBackgroundColor(Color);
    void SetDefaultFont(Font);
    void Draw();
};

Button::Button(Rectangle rec_)
    : rec(rec_) {}

Rectangle* Button::GetRec() { return &rec;}

inline bool Button::Clicked()
{
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) 
    && CheckCollisionPointRec(GetMousePosition(),rec);
}

void Button::SetText(const char* text)
{
    this -> texto = text;
}

void Button::SetBackgroundColor(Color color_)
{
    this -> color = color_;
}

void Button::SetDefaultFont(Font font_){
    font = font_; 
}

void Button::Draw()
{
    DrawRectangle(rec.x,rec.y,rec.width,rec.height,color);
    DrawRectangleLines(rec.x,rec.y,rec.width,rec.height,BLACK);

    Vector2 vec2 = MeasureTextEx(this->font,this->texto,24,1);
    vec2 = get_centered_position(vec2,rec);
    DrawTextEx(this->font,this->texto,vec2,24,1,BLACK);
}