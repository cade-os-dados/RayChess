#pragma once
#include <raylib.h>
#include <string>
#include "double_click.hpp"
#include "caret.hpp"
#include "ip_validator.hpp"

/*
    Futuramente deixar esta classe extensível 
    para adicionar os validadores - como o de ip
*/
class InputText{
private:
    IpString text;
    Rectangle rec{0,0,0,0};
    int max_chars{4};
    BlinkTextSelectionBar caret = BlinkTextSelectionBar(rec);
    DoubleClick double_click;
    bool edit{false}; // se true está no modo de edição
    Font fonte = GetFontDefault();
    int font_size = 24;
public:
    InputText(Rectangle rec_) : rec(rec_) {
        // altura do caret - a mesma da fonte
        // só dar update no caret se for por evento
        // para nao desperdiçar processamento
        caret.SetHeight(static_cast<float>(font_size));
    };
    void SetMaxChars(int max_){max_chars = max_;}
    Rectangle* GetRec(){return &rec;}
    std::string str(){return text.str();};
    void SetDefaultFont(Font font){ fonte = font; }

    inline bool is_number(int key)
    {
        return (key >= 48) && (key <= 57);
    }

    /* 
        Equivalente a (char)key == '.'
    */
    inline bool is_dot(int key){return key == 46; }

    void rcv()
    {
        int key = GetCharPressed();

        while(key > 0)
        {
            // com base na tabela ASCII...
            if((is_number(key) || is_dot(key)) && (!text.full()))
            {
                text.push((char)key);
            }

            key = GetCharPressed();
        }

        if(IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE))
        {
            if(!text.empty())
            {
                text.pop();
            }  
        }
    }

    void SetCaretPosition()
    {
        float caret_shift = 0.0;
        if(!text.empty())
        {
            size_t index = text.GetCaretSize();
            std::string txt = text.str();
            const char* measure_text;
            if(index != txt.size())
            {
                txt = txt.substr(0,index);
            }
            caret_shift = MeasureTextEx(fonte, txt.c_str(), this -> font_size, 1).x;
        }

        caret.UpdateX(rec.x+caret_shift);
        caret.UpdateY(rec.y);
    }

    /* Atualiza o modo de edição de texto */
    void UpdateEditStatus()
    {
        bool colision = CheckCollisionPointRec(GetMousePosition(),rec);
        if(!edit && double_click.Check() && colision)
            edit = true;

        // click outside
        if(!colision && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            edit = false;
    }

    void Draw()
    {
        this -> UpdateEditStatus();

        if(!text.empty())
        {
            // depois podemos criar uma opção na API
            // para escolher canonical ou com zeros
            std::string texto = text.str(); // canonical
            // std::string texto = text.format(); // show zeros
            DrawTextEx(fonte,texto.c_str(),{rec.x,rec.y},this -> font_size,1,BLACK);
        }
            

        if(edit)
        {
            this -> rcv();
            this -> SetCaretPosition();

            // depois adicionar uma funcionalidade se está on ou off
            caret.Draw();    
        }
    }
};