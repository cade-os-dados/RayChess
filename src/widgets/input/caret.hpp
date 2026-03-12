#pragma once
#include "raylib.h"

// tecnicamente o retangulo dentro dessa classe é só para saber onde jogar o caret
// depois podemos achar estratégias melhores para lidar com isto, talvez até mesmo passando diretamente o valor no Draw

/* 
    estudei um pouco aqui a classe

    A altura é praticamente fixa com base no tamanho da fonte
    Aqui seria interessante um mecanismo de eventos pois é algo que não precisa
    ser atualizado a todo instante

    Já o x muda a todo instante pois toda vez que uma letra é escrita
    o blinker anda

    O y futuramente também se adicionar suporte a multiline neste input-text
*/

class BlinkTextSelectionBar {
private:
    Rectangle rec; 
    Color color{BLACK};
public:
    BlinkTextSelectionBar(Rectangle rec_){rec = rec_;}
    void UpdateX(float x){rec.x=x;}
    void UpdateY(float y){rec.y=y;}
    void SetHeight(float h){rec.height = h;}
    Rectangle* GetRec(){return &rec;}

    void Debug()
    {
        DrawText(TextFormat("Height: %f X: %f Y: %f", rec.height, rec.x, rec.y),100,100,24,BLACK);
    }

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

        Debug();
    }
};
