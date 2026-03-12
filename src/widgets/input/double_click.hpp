#pragma once
#include <raylib.h>
#include <string>

class DoubleClick{
private:
    bool clicked{false};
    double start = GetTime();
    double sensibility = 0.4; // double click delay in seconds
public:
    DoubleClick(double sensibidade = 0.4) : sensibility(sensibidade) {}
    
    bool Check()
    {
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if(!clicked)
            {
                clicked = true;
                start = GetTime();
                return false;
            }
            double tempo_atual = GetTime();
            if(tempo_atual - start < sensibility)
                return true;
            else // reseta
            {
                start = GetTime();
                return false;
            }
        }
        return false;
    }
};