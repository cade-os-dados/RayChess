#pragma once
#include <raylib.h>
#include "player.hpp"
#include "highlight.hpp"

inline void render_points(int height, Game& game)
{
    DrawRectangle(200,height+10,20,20,GOLD);
    DrawRectangle(200,height+40,20,20,VIOLET);
    DrawText(game.GoldPoints(), 240, height+10, 20, WHITE);
    DrawText(game.VioletPoints(), 240, height+40, 20, WHITE);
}

inline void 
render_background_outside_board(int height, int width)
{
    DrawRectangle(0,height,width,100,DARKGRAY);
    // welcome - só de zueira
    DrawText("Welcome, hehehe", 620, height+10, 20, LIGHTGRAY);
}

inline void render_who_play(
    bool is_gold_turn, 
    int height, 
    int width)
{
    Color color; 
    const char* texto;
    if(is_gold_turn)
    {
        color = GOLD;
        texto = "GOLD";
    }else{
        color = VIOLET;
        texto = "VIOLET";
    }

    DrawText("QUEM JOGA: ", 10, height+10, 20, WHITE);
    DrawRectangle(10,height+40,20,20,color);
    DrawText(texto,40,height+40,20,WHITE);
}

inline void render_game(
    Board& board,
    bool is_gold_turn,
    pecas& gold, 
    pecas& violet,
    Game& game)
{
    int width, height;
    board.get_dimensions(&width, &height);

    BeginDrawing(); 
    ClearBackground(RAYWHITE);
    
    // BOARD
    board.Draw();
    // ---- PEÇAS ----
    for (const auto& peca : gold) { peca->Draw(); }
    for (const auto& peca : violet) { peca->Draw(); }

    // BACKGROUND
    render_background_outside_board(height,width);
    render_who_play(is_gold_turn, height, width);
    render_points(height, game);

    EndDrawing(); 
}

// HighLightControler c_highlight;
// bool is_gold_turn = true;

// NETWORK_SIDE SIDE = CLIENT_SIDE;

void render_game_scene(
    VecMatrixPosition& cache_possible_moves, 
    Board& board,
    pecas& gold,
    pecas& violet,
    Game& game,
    Player& player,
    bool& is_gold_turn,
    SyncMove& sync_move,
    CelDim cel,
    HighLightControler& c_highlight,
    NETWORK_SIDE net_side)
{
    bool endgame = false;
    bool synchronize = false;

    // ------------- RENDER ---------------
    render_game(board, is_gold_turn, gold, violet, game);
    // ------------- END RENDER -----------

    // lógica
    if(!player.is_turn)
    {
        MessageQueue* queue = (net_side == SERVER_SIDE) ? &request_queue : &response_queue;

        if(!queue -> empty()) // notificacao assincrona!
        {
            std::string message = queue -> pop();
            if(is_sync_move(message))
            {
                synchronize = true;
                sync_move = parse(message);
                printf("Move: %d -> (%d,%d)\n", sync_move.piece, sync_move.mov.row, sync_move.mov.col);
            }
        }
    }else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        
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
                if(net_side == SERVER_SIDE)
                    push_and_notify(to_str(sync_move));
                else
                    request_queue.push(to_str(sync_move));
                // request_queue.push(to_str(sync_move));
                c_highlight.Change(false);
            }
        }
        board.Debug();
    }

    if(synchronize)
    {
        endgame = player.Sync(game,sync_move,cel);
        if(net_side == SERVER_SIDE)
            player.ChangeTurn(endgame,&is_gold_turn,[](){});
        else
            player.ChangeTurn(endgame,&is_gold_turn,[&player](){
                if(!player.is_turn) request_queue.push("ping");
            });
    }
}
    