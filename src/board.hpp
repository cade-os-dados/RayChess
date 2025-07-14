#pragma once
#include "raylib.h"
#include <tuple>

/*
    1. Criar uma classe que transforma coordenadas da posição do mouse
    em coordenadas da matriz do tabuleiro...
*/

bool CompareColors(Color c1, Color c2) {
    return (c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b) && (c1.a == c2.a);
}

enum Action {
    attack,
    movable,
    blocked,
    unacessable
};

class Board {
private:
    int x, y, n, cels[8][8];
    Color celColors[8][8];
    Color originalCelColors[8][8];
public:
    Board(int w, int h, int len) : x(w), y(h), n(len) {
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                int value;

                if(i == 0) value = j+1;
                else if(i == 1) value = 8+j+1;
                else if(i == 7) value = -j-1;
                else if(i == 6) value = -8-j-1;
                else value = 0;

                // define
                cels[i][j] = value;
            }
        }
        setupColors();
    }
    void setupColors(void){
        int c = 0;
        for(int i = 0; i < n; i++) // cols
        {
            for(int j = 0; j < n; j++) // rows
            {
                if(n % 2 == 0 && i > 0 && j == 0)
                    c++;
                Color color = c % 2 == 0 ? WHITE : BLACK;
                celColors[i][j] = color;
                originalCelColors[i][j] = color;
                c++;
            }
        }
    }

    void Draw(void){
        for(int i = 0; i < n; i++) // cols
        {
            for(int j = 0; j < n; j++) // rows
            {
                // DrawRectangle(x*i, y*j, x, y, originalCelColors[j][i]);
                Color cor = celColors[j][i];
                Rectangle retangulo{(float)x*i, (float)y*j, (float)x, (float)y};
                // DrawRectangleLinesEx(retangulo,1.5,GRAY);
                DrawRectangleRoundedLinesEx(retangulo,0.1,20,1.7,GRAY);

                if(!CompareColors(cor,WHITE) && !CompareColors(cor,BLACK)){
                    Color cor2 = cor;
                    cor2.a = 150;
                    DrawRectangle(retangulo.x,retangulo.y,retangulo.width,retangulo.height,cor2);

                    // círculo
                    // DrawCircle(x*i + x/2, y*j + y/2, 10, cor);

                    // pulse effect
                    // int alpha = (int)(GetTime() * 255) % 255;
                    // Color pulseColor = {0, 255, 0, alpha};
                    // DrawRectangle(x*i, y*j, x, y, pulseColor);

                }else{
                    DrawRectangle(x*i, y*j, x, y, cor); 
                }
            }
                
        }
    }
    void RegisterPosition(int i, int k, int gold){
        cels[i][k] = gold;
    }
    Action VerifyPosition(int i, int k, int gold){
        if(i < 0 || k < 0 || i > 7 || k > 7)
            return Action::unacessable;
        int id = cels[i][k];
        if(id == 0) 
            return Action::movable;

        bool blocked = id > 0;
        blocked = gold ? blocked : !blocked;
        return blocked ? Action::blocked : Action::attack;
    }
    int CheckWhereCliked(void)
    {
        Vector2 pos = GetMousePosition();
        auto [a,b] = this->from_coord(pos.x,pos.y);
        return this->cels[a][b];
    } 

    int Where(int i, int j) {return cels[i][j]; }

    std::tuple<int, int> to_coord(int i, int j){
        return std::make_tuple(x*i, y*j);
    }
    std::tuple<int, int> trunc_coord(int i, int j){
        int width = (int)(i / x) * x;
        int height = (int)(j / y) * y;
        return std::make_tuple(width, height);
    }
    std::tuple<int, int> from_coord(int i, int j){
        auto [a,b] = trunc_coord(i, j);
        return std::make_tuple((int)b/y, (int)a/x);
    }

    void changeCellColor(int i, int j, Color color){
        celColors[i][j] = color;
    }
    void backupCellColor(int i, int j){
        celColors[i][j] = originalCelColors[i][j];
    }
    void backupAllCellColor(void){
        for(int p = 0; p < 8; p++){
            for(int k = 0; k < 8; k++) this->backupCellColor(p,k);
        }
    }

    std::tuple<int,int> onHover(Color cor){
        Vector2 mousePosition = GetMousePosition();
        // std::cout << "[Debug] - Position - X: " << mousePosition.x << " Y: " << mousePosition.y << std::endl;
        auto [a,b] = this->from_coord(mousePosition.x, mousePosition.y);
        std::cout << "[DEBUG] COORDS A: " << a << " B: " << b << std::endl; 
        this->changeCellColor(b,a,cor);
        return std::make_tuple(b,a);
    }
    auto getColor(int i, int j){
        return celColors[i][j];
    }

    void Highlight(std::vector<std::tuple<int,int>> possible_positions)
    {
        for (auto [xi,ji] : possible_positions){
            // std::cout << "(xi, ji): (" << xi << "," << ji << ")" << std::endl;
            Action acao = this -> VerifyPosition(xi,ji,1);
            if (acao == Action::movable){
                // std::cout << "Changing color: " << xi << "," << ji << std::endl;
                this -> changeCellColor(xi,ji,GREEN);
            } else if(acao == Action::attack) {
                this -> changeCellColor(xi,ji,RED);
            }
        }
    }

    /*------------------------------------------------------------------------------------------------*/
    /*-----------------------------------------------DEBUG--------------------------------------------*/
    void Debug(){
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                std::cout << this->cels[i][j]<<" ";
            }
            std::cout << std::endl;
        }
    }
    void Debug(int i, int j){
        std::cout << "Board coord: (" << i << "," << j << "): " << cels[i][j] << std::endl;
    }
    void DebugColor(void)
    {
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                auto c = this->celColors[i][j];
                char texto;
                if(CompareColors(c,GREEN)) texto = 'g';
                if(CompareColors(c,WHITE)) texto = 'w';
                if(CompareColors(c,BLACK)) texto = 'b';
                if(CompareColors(c,RED)) texto = 'r';
                if(CompareColors(c,GOLD)) texto = 'y';
                std::cout << texto <<" ";
            }
            std::cout << std::endl;
        }
    }
};

void debugAction(Action verify)
{
    switch (verify)
    {
    case Action::attack:
        std::cout << "Atack" << std::endl;
        break;
    case Action::blocked:
        std::cout << "Blocked" << std::endl;
        break;
    case Action::movable:
        std::cout << "Movable" << std::endl;
        break;
    case Action::unacessable:
        std::cout << "Unacessable" << std::endl;
        break;
    }
}