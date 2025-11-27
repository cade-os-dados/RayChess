#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "highlight.hpp"
#include "game.hpp"
#include "net/server.hpp"
#include "net/parser.hpp"

const int W = 800;
const int H = 600;
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
    2. Trancar a jogada adversária
    3. Ao realizar o movimento, enviar via tcp
    4. Esperar a resposta do adversário

    Acredito que seja mais fácil no mesmo arquivo ser server e player
    Do que rodar o server separado pois o server terá que lidar com restream digamos
*/

int main()
{
    // NET SERVER
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 4433));
    generate_ca();
    auto ctx = context_with_ca();

    Session session(io_context, acceptor, ctx);
    std::cout << "Waiting for connection...\n";
    session.handshake();
    // while(session.IsActive())
    // {
    //     std::string mensagem = session.rcv();
    //     std::cout << mensagem << std::endl;
    //     if(is_sync_move(mensagem))
    //     {
    //         SyncMove move = parse(mensagem);
    //         std::cout << "Move: " << move.piece << move.x << move.y << std::endl;
    //     }

    //     std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    //     session.write(make_daytime_string());
    // }





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

        if(is_player_turn)
        {
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

                            // send across network
                            if(session.IsActive()) 
                                session.write(to_str({piece_idx, x_mov, y_mov}));
                            
                            c_highlight.Change(false);
                            game.CheckEndGame(piece, is_gold_piece);
                            is_player_turn = false;
                            break;
                        }
                    }
                }
                board.Debug();
            }
        }else{
            // wait for response -> vai travar tudo
            // terá que rodar numa thread separada e esperar o evento disparar
            // std::string response = session.rcv();
            // std::cout << response << std::endl;
            if(notify) // notificacao assincrona!
                is_player_turn = true;
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}