/*
    Responsabilidades:
    
    Player 1 -> checar se 
*/
#pragma once
#include "game.hpp"
#include "net/parser.hpp"
#include "types.hpp"

class Player{
    PIECE_COLOR player_color, enemy_color;
public:
    bool is_turn;
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
    void Sync(Game& game, SyncMove sync, CelDim cel)
    {
        int piece_to_kill = game.Kill(sync.mov, enemy_color == PIECE_COLOR_GOLD);
        game.Move(sync.piece,sync.mov,cel);
        if(piece_to_kill > 0)
        {
            if(game.CheckEndGame(piece_to_kill, enemy_color == PIECE_COLOR_GOLD))
            {
                game.Reset();
                return;
            }
        }
    }
};

