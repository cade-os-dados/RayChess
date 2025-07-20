#pragma once
#include <vector>
#include "board.hpp"

#define INCREMENT_UNTIL(incRow,incCol,cond) \
    this->ResetParams(i,j,gold);\ 
    while(cond){\
    row+=incRow; col+=incCol;\
    if(this->AppendAndCheck(coords))\
        break;\
    }

typedef std::vector<std::tuple<int,int> > VecCoords;

void append_coords(VecCoords& vec, int i, int j){
    vec.push_back(std::make_tuple(i,j));
}

class InfinityMove{
private:
    Board* m_Board;
    int row, col, is_gold;
public:
    InfinityMove(Board* board) : m_Board(board) {}
    bool BreakCondition(int k, int l, int is_gold){
        return m_Board -> VerifyPosition(k,l,is_gold) != Action::movable;
    }
    bool AppendAndCheck(VecCoords& coords){
        append_coords(coords,row,col);
        return this->BreakCondition(row,col,this->is_gold);
    }
    void ResetParams(int i, int j, bool gold){
        this->row = i, this->col = j; this -> is_gold = (int)gold;
    }

    // podemos fazer um switch depois
    // com Enum
    void DiagonalLeftBottom(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(1, 1, row < 8 && col < 8);
    }

    void DiagonalLeftTop(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(1, -1, row < 8 && col >= 0);
    }

    void DiagonalRightBottom(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(-1, 1, row >= 0 && col < 8);
    }

    void DiagonalRightTop(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(-1, -1, row >= 0 && col >= 0);
    }

    void Top(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(-1,0,row >= 0);
    }
    void Bottom(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(1,0,row < 8);
    }

    void Left(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(0,-1,col >= 0);
    }
    void Right(VecCoords& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(0,1,col < 8);
    }
};