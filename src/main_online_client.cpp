#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "highlight.hpp"
#include "net/client.hpp"
#include "net/safe_queue.hpp"
#include "coord.hpp"
#include "player.hpp"

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

    O novo fluxo deve ser assim:
    Player1 -> fixo para todos os modos
    Player2 -> local, remote, IA

    Daí player1.play() -> logica de highlight, move/kill/checkreset
    player2.play() 
        -> se local idem player1
        -> se remote espera a mensagem e sincroniza
        -> se IA espera a IA e sincroniza
    
    Depois criamos a logica de seleção dos modos
    E a lógica de conexão
*/

MessageQueue request_queue, response_queue;
SyncMove sync_move;

void rede()
{  
    Client client(boost::asio::ssl::verify_none);
    client.connect("127.0.0.1","4433");
    client.handshake();
    std::cout << "handshake ok\n";

    
    client.write("ping");
    // std::cout << "Receveid: " << response << std::endl;
    // response_queue.push(response);

    std::thread receiver([&client](){
        while(true)
        {
            std::string response = client.rcv();
            std::cout << "Received: " << response << std::endl;
            response_queue.push(response);
        }
    });

    std::thread sender([&client](){
        while(true)
        {
            if(!request_queue.empty())
            {
                // Bloqueia e espera por uma mensagem na fila
                std::string msg_to_send = request_queue.pop(); 
                
                if (msg_to_send == "__STOP__") break;
                
                client.write(msg_to_send);
            }
        }
    });

    sender.join();
    receiver.join();
    client.run(); // O run só será chamado após o loop terminar
}


int main()
{
    // NET SERVER
    std::thread th(rede);

    Player player1(PIECE_COLOR_VIOLET);

    // ---- TABULEIRO ----
    int n = 8;
    int cel_width = (int) (W / n); // delegar ao board
    int cel_height = (int) (H / n); // delegar ao board
    Board board(cel_width, cel_height, n);
    InfinityMove move(&board);

    int rodada = 0;
    InitWindow(W, H+100, "Client");
    
    SetTargetFPS(60);
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    pecas gold = InitPecas(true, &move);
    pecas violet = InitPecas(false, &move);

    Color cartao_color = GOLD;

    VecMatrixPosition cache_possible_moves;
    c_highlight.SetPieces(&gold,true);
    c_highlight.SetPieces(&violet,false);
    c_highlight.SetBoardPtr(&board);

    Game game(&gold, &violet, &is_gold_turn, &board);
    
    // bool debug = true;
    while(!WindowShouldClose())
    {
        bool endgame = false;
        bool synchronize = false;

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
        EndDrawing(); 

        // lógica
        if(!player1.is_turn)
        {
            if(!response_queue.empty()) // notificacao assincrona!
            {
                std::string message = response_queue.pop();
                if(is_sync_move(message))
                {
                    synchronize = true;
                    sync_move = parse(message);
                    printf("Move: %d -> (%d,%d)\n", sync_move.piece, sync_move.mov.row, sync_move.mov.col);
                }
            }
        }else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            rodada++;
            std::cout << "------------------------------\nRodada: " << rodada << std::endl;
            
            c_highlight.UpdateClicked(true);
            Vector2 mousePosition = GetMousePosition();
            int where_clicked = board.CheckWhereCliked();
            c_highlight.setHighlight(where_clicked, is_gold_turn, cache_possible_moves);

            if(c_highlight.is_on())
            {
                if(c_highlight.CheckReHighlight(where_clicked))
                {
                    c_highlight.ReHighlight(where_clicked, is_gold_turn, cache_possible_moves);
                }
                
                MatrixPosition new_pos = game.GetMatrixPos(mousePosition);

                if(ContainsMatrixPos(cache_possible_moves, new_pos))
                {
                    synchronize = true;

                    int piece_idx = c_highlight.getPieceIndex();
                    // send across network
                    sync_move = {piece_idx, new_pos};
                    request_queue.push(to_str(sync_move));
                    c_highlight.Change(false);
                }
            }
            board.Debug();
        }

        if(synchronize)
        {
            endgame = player1.Sync(game,sync_move,cel);
            player1.ChangeTurn(endgame,&is_gold_turn,[&player1](){
                if(!player1.is_turn) request_queue.push("ping"); // servidor é síncrono, apenas para sincronizar
            });
        }
    }

    th.join();
    CloseWindow();
    return 0;
}