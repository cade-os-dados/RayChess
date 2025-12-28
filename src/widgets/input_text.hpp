#pragma once
#include <raylib.h>
#include <string>

#define DOUBLE_CLICK_DELAY 0.4

class DoubleClick{
private:
    bool clicked{false};
    double start = GetTime();
public:
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
            float tempo_atual = GetTime();
            if(tempo_atual - start < DOUBLE_CLICK_DELAY)
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

class BlinkTextSelectionBar {
private:
    Rectangle rec;
    Color color{BLACK};
public:
    BlinkTextSelectionBar(Rectangle rec_){rec = rec_;}
    void UpdateX(float x){rec.x=x;}
    void UpdateY(float y){rec.y=y;}
    void UpdateHeight(float h){rec.height = h;}
    Rectangle* GetRec(){return &rec;}

    void Draw()
    {
        // Lógica dentro do loop 'Update'
    float tempo_atual = GetTime();

    // Se o resto da divisão do tempo atual por 1.0 (segundo) for menor que 0.5 (meio segundo), desenhe.
    if ((int)(tempo_atual * 2) % 2 == 0)
    {
        // Desenha o retângulo (caret)
        DrawRectangle(
            rec.x, // Posição X
            rec.y,                    // Posição Y
            2,                               // Largura (2 pixels)
            rec.height,                 // Altura
            color                         // Cor
        );
    }
    }
};

class InputText{
private:
    std::string text;
    Rectangle rec{0,0,0,0};
    int max_chars{4};
    BlinkTextSelectionBar selectionbar = BlinkTextSelectionBar(rec);
    DoubleClick double_click;
    bool edit{false};
    bool show_only_blink{false};
    Font fonte = GetFontDefault();
public:
    InputText(Rectangle rec_) : rec(rec_) {};
    void SetMaxChars(int max_){max_chars = max_;}
    Rectangle* GetRec(){return &rec;}
    int getValue(){return std::atoi(text.data());}
    const char* str(){return text.data();};
    int size(){return text.size();};
    void SetDefaultFont(Font font){ fonte = font; }

    void rcv()
    {
        int key = GetCharPressed();

        while(key > 0)
        {
            // com base na tabela ASCII...
            if((key >= 48) && (key <= 57) && (text.size() < max_chars))
            {
                text.push_back((char)key);
            }

            key = GetCharPressed();
        }

        if(IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))
        {
            if(text.size() > 0)
            {
                text.pop_back();
            }  
        }
    }

    void UpdateBlinker()
    {
        const char* measurable_text = text.size() > 0 ? text.data() : "0";
        Vector2 r = MeasureTextEx(fonte, measurable_text, 24, 1);
        selectionbar.UpdateHeight(r.y);
        selectionbar.UpdateX(rec.x+r.x);
        selectionbar.UpdateY(rec.y);
    }

    void UpdateEditStatus()
    {
        bool colision = CheckCollisionPointRec(GetMousePosition(),rec);
        if(!edit && double_click.Check() && colision)
            edit = true;

        // click outside
        if(!colision && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            edit = false;
    }

    void Plus(int plus)
    {
        int valor = this -> getValue();
        text = std::to_string(valor+plus);
    }

    void Minus(int minus)
    {
        int valor = this -> getValue();
        text = std::to_string(valor-minus);
    }

    void Draw()
    {
        this -> UpdateEditStatus();

        DrawRectangleLines(rec.x,rec.y,rec.width,rec.height,BLACK);

        if(text.size() > 0)
            DrawTextEx(fonte,text.data(),{rec.x,rec.y},24,1,BLACK);

        if(edit)
        {
            this -> rcv();
            this -> UpdateBlinker();

            // depois adicionar uma funcionalidade se está on ou off
            selectionbar.Draw();    
        }
    }
};