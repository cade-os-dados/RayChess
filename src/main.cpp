#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "pieces.hpp"
#include "board.hpp"
#include "highlight.hpp"

/*
    Próximos passos
    1. Eliminar peças inimigas (vai precisar de um controler / ID por peça)
    ideia: ter um vetor com 
*/

const int W = 800;
const int H = 600;
HighLightControler c_highlight;

using namespace std;
typedef vector<unique_ptr<Peca>> pecas;
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

            c_highlight.UpdateClicked(true);

            Vector2 mousePosition = GetMousePosition();
            std::cout << "[Debug] - Position - X: " << mousePosition.x << " Y: " << mousePosition.y << std::endl;
            auto [a,b] = board.from_coord(mousePosition.x, mousePosition.y);

            //debug
            Action verify = board.VerifyPosition(a, b, 1);
            std::cout << "i: " << a << "j: " << b << std::endl;
            debugAction(verify);
            
            if (a==i && b == j){
                c_highlight.Change(true);
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
                // debug
                board.Debug();
                std::cout << "\nColors:\n";
                board.DebugColor();
                
            }else { 
                c_highlight.UpdateClicked(false);
            }

            if(c_highlight.is_on())
            {
                if (c_highlight.Unhighlight())
                {
                    board.backupAllCellColor();
                    c_highlight.Change(false);
                }
                for (auto [xi,ji] : cache_possible_moves){
                    if(a == xi && b == ji){
                        auto [xk,jk] = board.trunc_coord(mousePosition.x,mousePosition.y);
                        auto [act_x, act_y] = lg_cavalo.coords();
                        auto [act_xx,act_yy] = board.from_coord(act_x,act_y);
                        board.RegisterPosition(act_xx, act_yy,0);
                        std::cout << "(xi,ji): (" << xi << "," << ji << ")" << std::endl;
                        
                        int piece = board.Where(xi,ji);
                        if(piece == -5) violet.erase(violet.begin() + 4); // exemplo de como deleter o rei
                        // porem ao deletar uma peça a ordem dos índices muda...
                        // precisamos pensar em algo para resolver isso
                        // uma possibilidade seria trocar por uma "peça nula" em que Draw não faz nada
                        // por que basicamente é pra isso que serve o vetor... (até o momento)
                        
                        lg_cavalo.Move({ (float)xk, (float)jk });
                        board.RegisterPosition(xi,ji,1);
                        c_highlight.Change(false);
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