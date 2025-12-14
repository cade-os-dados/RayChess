/*
    Responsabilidades:
    
    Player 1 -> checar se 
*/
#pragma once
#include "game.hpp"
#include "net/parser.hpp"
#include "types.hpp"
#include <functional>

class Player{
public:
    PIECE_COLOR player_color, enemy_color;
    bool is_turn;
    int num_resets{0};
    Player(PIECE_COLOR color)
    {
        player_color = color;
        enemy_color == PIECE_COLOR_GOLD ? PIECE_COLOR_VIOLET : PIECE_COLOR_GOLD;
        if(player_color == PIECE_COLOR_GOLD)
        {
            enemy_color = PIECE_COLOR_VIOLET;
            this -> is_turn = true;
        }else{
            enemy_color = PIECE_COLOR_GOLD;
            this -> is_turn = false;
        }
    }
    bool Sync(Game& game, SyncMove sync, CelDim cel)
    {
        PIECE_COLOR color = is_turn ? player_color : enemy_color;
        bool is_gold = color == PIECE_COLOR_GOLD;
        
        int piece_to_kill = game.Kill(sync.mov, is_gold);
        game.Move(sync.piece,sync.mov,cel);
        if(game.CheckEndGame(piece_to_kill, is_gold))
        {
            game.Reset();
            return true;
        }
        return false;
    }

    void ChangeTurn(bool endgame, 
        bool* is_gold_turn, 
        std::function<void(void)> callback)
    {
        if(!endgame)
        {
            *is_gold_turn = !(*is_gold_turn);
            is_turn = !is_turn;
        }else{
            callback();
            *is_gold_turn = true;
            is_turn = player_color == PIECE_COLOR_GOLD;
        }
    }
};

