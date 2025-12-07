#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "highlight.hpp"
#include "game.hpp"
#include "net/sync_server.hpp"
#include "net/parser.hpp"
#include "coord.hpp"

const int W = 800;
const int H = 600;
CelDim cel = to_cel_dim({W,H});
HighLightControler c_highlight;
bool is_gold_turn = true;

using namespace std;

/*
    Por enquanto façamos separado este arquivo
    futuramente desenhamos uma solução com modos de jogo:

    1. singleplayer
    2. contra IA (beeem futuramente)
    3. Online

    Por enquanto o que precisamos:

    1. Selecionar com qual tabuleiro vai jogar (gold/violet)

    2. Criar o client agora, já temos o server

    O ideal é antes de inicializar o servidor tcp, ter nos menus as opções
    -> client ou servidor
    -> se servidor selecionar a porta
    -> se client, digitar ip e porta

    3. Passar as principais funções para a nova api
    acabar com tuple<int,int> que é meio sem significado e confuso

    Vamos criar uma api player que facilite isso, pois assim 
*/

int main()
{
    // NET SERVER
    std::thread th([]{
        boost::asio::io_context io_context;
        Server s(io_context, 4433);
        io_context.run();
    }); // io_context.run();

    // ---- TABULEIRO ----
    int n = 8;
    int cel_width = (int) (W / n); // delegar ao board
    int cel_height = (int) (H / n); // delegar ao board
    Board board(cel_width, cel_height, n);
    InfinityMove move(&board);

    int rodada = 0;
    InitWindow(W, H+100, "Chess!");
    
    SetTargetFPS(60);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    pecas gold = InitPecas(true, &move);
    pecas violet = InitPecas(false, &move);

    bool is_player_turn = true;

    Color cartao_color = GOLD;

    VecMatrixPosition cache_possible_moves;
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

        if(!is_player_turn)
        {
            if(!request_queue.empty()) // notificacao assincrona!
            {
                std::string message = request_queue.pop();
                if(is_sync_move(message))
                {
                    // parse
                    SyncMove sync = parse(message);
                    printf("Move: %d -> (%d,%d)\n", sync.piece, sync.mov.row, sync.mov.col);

                    int piece_to_kill = game.Kill(sync.mov, false);
                    game.Move(sync.piece, sync.mov, cel);

                    // check endgame
                    if(piece_to_kill > 0)
                        game.CheckEndGame(piece_to_kill, false);

                }else{printf("Mensagem rejeitada\n");}
                // depois revemos isto
                // se a mensagem for passada errada perde a vez
                // mas por agora é importante para eu sincronizar na primeira jogada...
                is_player_turn = true;
                is_gold_turn = true;
            }
        }else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
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

                for (MatrixPosition new_pos : cache_possible_moves)
                {
                    if(game.CanMove(new_pos))
                    {
                        auto [piece_ptr, piece_idx, is_gold_piece] = c_highlight.GetInfo();
                        int piece = game.Kill(new_pos, is_gold_piece);
                        game.Move(piece_idx, new_pos, cel);

                        // send across network
                        push_and_notify(to_str({piece_idx,new_pos.row,new_pos.col}));
                        
                        c_highlight.Change(false);
                        game.CheckEndGame(piece, is_gold_piece);
                        is_player_turn = false;
                        break;
                    }
                }
            }
            board.Debug();
        }
        EndDrawing();
    }

    th.join();
    CloseWindow();
    return 0;
}