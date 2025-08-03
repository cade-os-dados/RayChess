#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
// #include "board.hpp"
#include "highlight.hpp"

const int W = 800;
const int H = 600;
HighLightControler c_highlight;
bool is_gold_turn = true;

using namespace std;
// typedef vector<shared_ptr<Peca>> pecas;
pecas InitPecas(bool is_gold, InfinityMove* move)
{
    pecas pecas;
    pecas.push_back(make_shared<Torre>(is_gold, true, move));
    pecas.push_back(make_shared<Cavalo>(is_gold, true, move));
    pecas.push_back(make_shared<Bispo>(is_gold,true, move)); 
    pecas.push_back(make_shared<Rainha>(is_gold, move));
    pecas.push_back(make_shared<Rei>(is_gold, move));
    pecas.push_back(make_shared<Bispo>(is_gold, false, move));
    pecas.push_back(make_shared<Cavalo>(is_gold, false, move));
    pecas.push_back(make_shared<Torre>(is_gold, false, move));
    for(int i = 0; i < 8; i++){
        pecas.push_back(make_shared<Peao>(is_gold,i,move)); // PEAO
    }
    return pecas;
}

int main()
{
    // ---- TABULEIRO ----
    
    int n = 8;
    int cel_width = (int) (W / n);
    int cel_height = (int) (H / n);
    Board board(cel_width, cel_height, n);
    InfinityMove move(&board);

    int rodada = 0;
    InitWindow(W, H+100, "Chess!");
    
    SetTargetFPS(60);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    pecas gold = InitPecas(true, &move);
    pecas violet = InitPecas(false, &move);
    pecas gold_eliminados;
    pecas violet_eliminados;
    Color cartao_color = GOLD;

    std::vector<std::tuple<int,int>> cache_possible_moves;
    c_highlight.SetPieces(&gold,true);
    c_highlight.SetPieces(&violet,false);
    
    // bool debug = true;
    while(!WindowShouldClose())
    {
        BeginDrawing(); 
        ClearBackground(RAYWHITE);
        
        board.Draw();
        // ---- PEÇAS ----
        for (const auto& peca : gold) { if(peca != NULL) peca->Draw(); }
        for (const auto& peca : violet) { if(peca != NULL) peca->Draw(); }

        /* MENU EM BAIXO DO TABULEIRO */
        cartao_color = is_gold_turn ? GOLD : VIOLET;
        const char* texto = is_gold_turn ? "GOLD" : "VIOLET";
        // em baixo do tabuleiro...
        DrawRectangle(0,H,W,100,DARKGRAY);
        // welcome
        DrawText("Welcome, hehehe", 620, H+10, 20, LIGHTGRAY);
        // cartao
        DrawText("QUEM JOGA: ", 10, H+10, 20, WHITE);
        DrawRectangle(10,H+40,20,20,cartao_color);
        DrawText(texto,40,H+40,20,WHITE);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            rodada++;
            std::cout << "------------------------------\nRodada: " << rodada << std::endl;
            
            c_highlight.UpdateClicked(true);

            Vector2 mousePosition = GetMousePosition();
            std::cout << "[Debug] - Position - X: " << mousePosition.x << " Y: " << mousePosition.y << std::endl;
            auto [a,b] = board.from_coord(mousePosition.x, mousePosition.y);
            
            // exemplo - clicou fora
            int where_clicked = board.CheckWhereCliked();

            // caso gold
            if(!c_highlight.is_on() && where_clicked > 0 && is_gold_turn){
                c_highlight.Change(true);
                c_highlight.setPieceIndex(where_clicked);
                auto [x,y] = c_highlight.getPiece() -> coords();
                auto [i,j] = board.from_coord(x,y);
                cache_possible_moves = c_highlight.getPiece() -> PossibleMoveCoords(i,j);
                board.Highlight(cache_possible_moves, 1);
                
                /* Se não há movimentos possíveis para a peça, então sequer aciona o sistema de highlight... */
                if (cache_possible_moves.empty()){
                    c_highlight.Change(false);
                }
            }

            // caso violet
            if(!c_highlight.is_on() && where_clicked < 0 && !is_gold_turn){
                c_highlight.Change(true);
                c_highlight.setPieceIndex(where_clicked);
                auto [x,y] = c_highlight.getPiece() -> coords();
                auto [i,j] = board.from_coord(x,y);
                cache_possible_moves = c_highlight.getPiece() -> PossibleMoveCoords(i,j);
                board.Highlight(cache_possible_moves, 0);

                if (cache_possible_moves.empty()){
                    c_highlight.Change(false);
                }
            }


            if(c_highlight.is_on())
            {
                if(c_highlight.DoubleClickedOnPiece(where_clicked) || 
                    where_clicked != c_highlight.getPieceIndex() 
                    // !c_highlight.ClickedSameTeam(where_clicked)
                ){
                    std::cout << "Double Clicked?: " << c_highlight.DoubleClickedOnPiece(where_clicked) << std::endl;
                    std::cout << "ClickedSameTeam?: " << c_highlight.ClickedSameTeam(where_clicked) << std::endl;
                    c_highlight.UpdateClicked(false);
                }


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
                        
                        int piece = board.Where(xi,ji);
                        if(piece < 0 && c_highlight.IsGold())
                            violet[abs(piece)-1] = NULL;
                        if(piece > 0 && !c_highlight.IsGold())
                            gold[piece-1] = NULL;

                        c_highlight.getPiece() -> Move({(float)xk, (float)jk});
                        board.RegisterPosition(xi,ji,c_highlight.getPieceIndex());
                        c_highlight.Change(false);
                        is_gold_turn = !is_gold_turn;
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