#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "highlight.hpp"
// #include "net/interface.hpp"
#include "coord.hpp"
#include "player.hpp"
#include "render.hpp"
#include "menu.hpp"
#include "scenes.hpp"

const int W = 800;
const int H = 600;
CelDim cel = to_cel_dim({W,H});
HighLightControler c_highlight;
bool is_gold_turn = true;

NETWORK_SIDE SIDE = SERVER_SIDE;
SyncMove sync_move;

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

/*
    ------------------------------- IMPORTANTE ------------------------------------
    MODO ONLINE... este é o modo que estou criando atualmente

    * Seleção de server e client em uma cena específica para isto
    * Seleção da cor do tabuleiro que se quer
    * Menu de continue somente no client...
    * (Futuramente) caso um tome disconect tem que lidar com isso automaticamente
*/


int main()
{
    // escopo
    {
        Player player1(PIECE_COLOR_GOLD);

        // ---- TABULEIRO ----
        int n = 8;
        int cel_width = (int) (W / n); // delegar ao board
        int cel_height = (int) (H / n); // delegar ao board
        Board board(cel_width, cel_height, n);
        InfinityMove move(&board);

        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(W, H+100, "Client");
        SetTargetFPS(60);
        
        pecas gold = InitPecas(true, &move);
        pecas violet = InitPecas(false, &move);

        VecMatrixPosition cache_possible_moves;
        c_highlight.SetPieces(&gold,true);
        c_highlight.SetPieces(&violet,false);
        c_highlight.SetBoardPtr(&board);

        Game game(&gold, &violet, &is_gold_turn, &board);

        SCENE scene = MENU_SCENE;
        
        while(!WindowShouldClose())
        {
            switch(scene)
            {
                case MENU_SCENE: scene = render_menu_scene(); break;
                case GAME_SCENE: scene = render_game_scene(cache_possible_moves,board,gold,violet,game,player1,is_gold_turn,sync_move,cel,c_highlight,SIDE); break;
                case CONTINUE_SCENE: scene = render_continue_scene(); break;
            }
        }
    } // call ~Peca() to unload texture

    CloseWindow();
    return 0;
}