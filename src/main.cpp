#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "board.hpp"
#include "highlight.hpp"

const int W = 800;
const int H = 600;
HighLightControler c_highlight;

using namespace std;
typedef vector<shared_ptr<Peca>> pecas;
pecas InitPecas(bool is_gold)
{
    pecas pecas;
    pecas.push_back(make_shared<Torre>(is_gold, true));
    pecas.push_back(make_shared<Cavalo>(is_gold, true));
    pecas.push_back(make_shared<Bispo>(is_gold,true)); 
    pecas.push_back(make_shared<Rainha>(is_gold));
    pecas.push_back(make_shared<Rei>(is_gold));
    pecas.push_back(make_shared<Bispo>(is_gold, false));
    pecas.push_back(make_shared<Cavalo>(is_gold, false));
    pecas.push_back(make_shared<Torre>(is_gold, false));
    for(int i = 0; i < 8; i++){
        pecas.push_back(make_shared<Peao>(is_gold,i)); // PEAO
    }
    return pecas;
}

int main()
{
    int rodada = 0;
    InitWindow(W, H, "Chess!");
    SetTargetFPS(60);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    pecas gold = InitPecas(true);
    pecas violet = InitPecas(false);
    pecas gold_eliminados;
    pecas violet_eliminados;

    std::vector<std::tuple<int,int>> cache_possible_moves;

    // ---- TABULEIRO ----
    
    int n = 8;
    int cel_width = (int) (W / n);
    int cel_height = (int) (H / n);
    Board board(cel_width, cel_height, n);

    // bool debug = true;
    while(!WindowShouldClose())
    {
        BeginDrawing(); 
        ClearBackground(RAYWHITE);
        
        board.Draw();
        // ---- PEÇAS ----
        for (const auto& peca : gold) { if(peca != NULL) peca->Draw(); }
        for (const auto& peca : violet) { if(peca != NULL) peca->Draw(); }

        // ---- TETANDO MOVIMENTO ----

        // texto aleatorio
        DrawText("Welcome, hehehe", 190, 200, 20, LIGHTGRAY);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            rodada++;
            std::cout << "------------------------------\nRodada: " << rodada << std::endl;

            c_highlight.UpdateClicked(true);

            Vector2 mousePosition = GetMousePosition();
            std::cout << "[Debug] - Position - X: " << mousePosition.x << " Y: " << mousePosition.y << std::endl;
            auto [a,b] = board.from_coord(mousePosition.x, mousePosition.y);

            /*
                Alterando a lógica
                1. Verifica qual a peça onde foi clicado...
                2. Se highlight -> verifica se move, attack, ou clicou fora
                3. Se não highlight e clicar em alguma peça
                verificar qual a peça, verificar suas coordenadas de movimento
                e dar highlight nestas coordenadas...
            */
            
            // exemplo - clicou fora
            int where_clicked = board.CheckWhereCliked();
            std::cout << "Onde clicou: " << where_clicked << std::endl;
            if(!c_highlight.is_on() && where_clicked == 0)
                std::cout << "Clicou no nada" << std::endl;

            // caso gold
            if(!c_highlight.is_on() && where_clicked > 0){
                c_highlight.Change(true);
                c_highlight.HighlightedColorIsGold(true);
                c_highlight.setPiece(gold[where_clicked-1]);
                c_highlight.setPieceIndex(where_clicked);
                auto [x,y] = c_highlight.getPiece() -> coords();
                auto [i,j] = board.from_coord(x,y);
                cache_possible_moves = c_highlight.getPiece() -> PossibleMoveCoords(i,j);
                board.Highlight(cache_possible_moves);
            }

            if(c_highlight.is_on())
            {
                if(where_clicked != c_highlight.getPieceIndex())
                    c_highlight.UpdateClicked(false);

                if (c_highlight.Unhighlight())
                {
                    board.backupAllCellColor();
                    c_highlight.Change(false);
                }

                for (auto [xi,ji] : cache_possible_moves){
                    if(a == xi && b == ji){
                        auto [xk,jk] = board.trunc_coord(mousePosition.x,mousePosition.y);
                        auto [act_x, act_y] = c_highlight.getPiece() -> coords();
                        auto [act_xx,act_yy] = board.from_coord(act_x,act_y);
                        board.RegisterPosition(act_xx, act_yy, 0);
                        // std::cout << "(xi,ji): (" << xi << "," << ji << ")" << std::endl;
                        
                        int piece = board.Where(xi,ji);
                        // verificar qual peça foi atacada...
                        if(piece < 0)
                            violet[abs(piece)-1] = NULL;

                        c_highlight.getPiece() -> Move({(float)xk, (float)jk});
                        board.RegisterPosition(xi,ji,c_highlight.getPieceIndex());
                        c_highlight.Change(false);
                        break;
                    }
                }
            }

            board.Debug();
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}