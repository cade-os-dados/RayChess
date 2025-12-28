#pragma once
#include <raylib.h>

// C++
Vector2 operator-(const Vector2& a, const Vector2& b) { return { a.x - b.x, a.y - b.y }; } 

inline Vector2 rec_center(Rectangle rec)
{
    Vector2 vec;
    vec.x = rec.x + rec.width/2;
    vec.y = rec.y + rec.height/2;
    return vec;
}

inline void set_text_centered(Vector2& pos, Rectangle to_center)
{
    
}

inline Vector2 get_centered_position(Vector2& measured_text, Rectangle to_center)
{
    Vector2 centered = rec_center(to_center);
    Vector2 half_text = {measured_text.x/2.0f,measured_text.y/2.0f};
    return centered - half_text;
}