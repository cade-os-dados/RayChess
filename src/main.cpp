#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "pieces.hpp"
#include "board.hpp"

/*
    Agora lógica para dar highlight nas peças:
    1. verificar se alguma peça foi clicada
    2. verificar os caminhos dessa peça, e quais caminhos são válidos (move ou attack)
    3. mudar a cor do fundo para os caminhos válidos
    4. armazenar em cache a cor original
    5. voltar a cor original após a jogada ou após o usuário clicar fora...
    6. armazenar em cache as possiveis jogadas quando está com highlight on
*/

const int W = 800;
const int H = 600;
int last_hover[2];
bool last_click = false;
bool clicked = false;

using namespace std;
vector<unique_ptr<Peca>> InitPecas(bool is_gold)
{
    vector<unique_ptr<Peca>> pecas;
    pecas.push_back(make_unique<Peao>(is_gold)); // PEAO
    // pecas.push_back(make_unique<Cavalo>(is_gold)); // CAVALO
    pecas.push_back(make_unique<Torre>(is_gold)); // TORRE
    pecas.push_back(make_unique<Bispo>(is_gold)); // BISPO
    pecas.push_back(make_unique<Rainha>(is_gold)); // RAINHA
    pecas.push_back(make_unique<Rei>(is_gold)); // REI
    return pecas;
}

int main()
{
    int rodada = 0;
    InitWindow(W, H, "Chess!");
    SetTargetFPS(60);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    vector<unique_ptr<Peca>> gold = InitPecas(true);
    vector<unique_ptr<Peca>> violet = InitPecas(false);
    std::vector<std::tuple<int,int>> cache_possible_moves;

    UniqueCavalo lg_cavalo(true, true);
    
    // ---- TABULEIRO ----
    
    int n = 8;
    int cel_width = (int) (W / n);
    int cel_height = (int) (H / n);
    Board board(cel_width, cel_height, n);

    bool debug = true;
    while(!WindowShouldClose())
    {
        
        BeginDrawing(); 
        ClearBackground(RAYWHITE);
        
        board.Draw();
        // ---- PEÇAS ----
        for (const auto& peca : gold) { peca->Draw(); }
        for (const auto& peca : violet) { peca->Draw(); }

        // ---- TETANDO MOVIMENTO ----
        lg_cavalo.Draw();
        // lg_cavalo.MoveOnClick();
        board.changeCellColor(0,0,GOLD);

        // texto aleatorio
        DrawText("Welcome, hehehe", 190, 200, 20, LIGHTGRAY);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            rodada++;
            std::cout << "------------------------------\nRodada: " << rodada << std::endl;

            // coordenadas do cavalo
            auto [x,y] = lg_cavalo.coords();
            auto [i,j] = board.from_coord(x,y);

            last_click = clicked;
            clicked = true;

            Vector2 mousePosition = GetMousePosition();
            std::cout << "[Debug] - Position - X: " << mousePosition.x << " Y: " << mousePosition.y << std::endl;
            auto [a,b] = board.from_coord(mousePosition.x, mousePosition.y);
            Action verify = board.VerifyPosition(a, b, 1);
            std::cout << "i: " << a << "j: " << b << std::endl;
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
            

            if (a==i && b == j){
                clicked=true;
                board.changeHighlight(true);
                // debug
                std::cout << "Clicou no cavalo, no quadrante: (" << i << "," 
                << j << ")" << std::endl;

                cache_possible_moves = lg_cavalo.movableCoords(i,j);
                for (auto [xi,ji] : cache_possible_moves){
                    std::cout << "(xi, ji): (" << xi << "," << ji << ")" << std::endl;
                    Action acao = board.VerifyPosition(xi,ji,1);
                    if (acao == Action::movable){
                        std::cout << "Changing color: " << xi << "," << ji << std::endl;
                        
                        board.changeCellColor(xi,ji,GREEN);
                    } else if(acao == Action::attack) {
                        board.changeCellColor(xi,ji,RED);
                    }
                }
                // board.Debug();
                // std::cout << "\nColors:\n";
                // board.DebugColor();
                
            }else { last_click = clicked; clicked = false; }

            if(board.highlight_on())
            {
                // unhighlight
                if (clicked == false)
                {
                    for(int p = 0; p < 8; p++){
                        for(int k = 0; k < 8; k++) board.backupCellColor(p,k);
                    }
                    board.changeHighlight(false);
                }
                for (auto [xi,ji] : cache_possible_moves){
                    if(a == xi && b == ji){
                        auto [xk,jk] = board.trunc_coord(mousePosition.x,mousePosition.y);
                        auto [act_x, act_y] = lg_cavalo.coords();
                        auto [act_xx,act_yy] = board.from_coord(act_x,act_y);
                        board.RegisterPosition(act_xx, act_yy,-1);
                        lg_cavalo.Move({ (float)xk, (float)jk });
                        board.RegisterPosition(xi,ji,1);
                        board.changeHighlight(false);
                        break;
                    }
                }
            }
           
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}