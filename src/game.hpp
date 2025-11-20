#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "highlight.hpp"
#include "points.hpp"

class Game
{
private:
    Board board;
    HighLightControler c_highlight;
    bool is_gold_turn = true;
    std::vector<std::tuple<int,int>> cache_possible_moves;
public:
    void process_highlight(int where_clicked)
    {
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
    } 
};