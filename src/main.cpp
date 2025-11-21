#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "highlight.hpp"
#include "game.hpp"

const int W = 800;
const int H = 600;
HighLightControler c_highlight;
bool is_gold_turn = true;

using namespace std;

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
    Color cartao_color = GOLD;

    std::vector<std::tuple<int,int>> cache_possible_moves;
    c_highlight.SetPieces(&gold,true);
    c_highlight.SetPieces(&violet,false);
    c_highlight.SetBoardPtr(&board);

    Game game(&gold, &violet, &is_gold_turn, &board);
    
    // bool debug = true;
    while(!WindowShouldClose())
    {
        BeginDrawing(); 
        ClearBackground(RAYWHITE);
        
        board.Draw();
        // ---- PEÇAS ----
        for (const auto& peca : gold) { peca->Draw(); }
        for (const auto& peca : violet) { peca->Draw(); }

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

        DrawRectangle(200,H+10,20,20,GOLD);
        DrawRectangle(200,H+40,20,20,VIOLET);
        DrawText(game.GoldPoints(), 240, H+10, 20, WHITE);
        DrawText(game.VioletPoints(), 240, H+40, 20, WHITE); 

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            rodada++;
            std::cout << "------------------------------\nRodada: " << rodada << std::endl;
            
            c_highlight.UpdateClicked(true);

            Vector2 mousePosition = GetMousePosition();
            std::cout << "[Debug] - Position - X: " << mousePosition.x << " Y: " << mousePosition.y << std::endl;
            
            int where_clicked = board.CheckWhereCliked();
            c_highlight.setHighlight(where_clicked, is_gold_turn, cache_possible_moves);

            if(c_highlight.is_on())
            {
                if(c_highlight.CheckReHighlight(where_clicked))
                {
                    c_highlight.ReHighlight(where_clicked, is_gold_turn, cache_possible_moves);
                }
                
                game.GetMatrixPos(mousePosition);
                game.GetTruncatedPos(mousePosition);

                for (auto [x_mov,y_mov] : cache_possible_moves)
                {
                    if(game.CanMove(x_mov, y_mov))
                    {
                        auto [piece_ptr, piece_idx, is_gold_piece] = c_highlight.GetInfo();
                        int piece = game.Kill(is_gold_piece);
                        game.Move(piece_ptr, piece_idx);
                        c_highlight.Change(false);
                        game.CheckEndGame(piece, is_gold_piece);
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