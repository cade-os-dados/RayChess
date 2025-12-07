#pragma once
#include <vector>
#include "board.hpp"
#include "coord.hpp"
#include "types.hpp"

#define INCREMENT_UNTIL(incRow,incCol,cond) \
    this->ResetParams(i,j,gold);\ 
    while(cond){\
    row+=incRow; col+=incCol;\
    if(this->AppendAndCheck(coords))\
        break;\
    }

void append_coords(VecMatrixPosition& vec, int i, int j){
    vec.push_back({i,j});
}

Action InfinityMove::VerifyPosition(int k, int l, bool gold){
    return m_Board -> VerifyPosition(k,l,(int)gold);
}
bool InfinityMove::BreakCondition(int k, int l, int is_gold){
    return m_Board -> VerifyPosition(k,l,is_gold) != Action::movable;
}
bool InfinityMove::AppendAndCheck(VecMatrixPosition& coords){
    append_coords(coords,row,col);
    return this->BreakCondition(row,col,this->is_gold);
}
void InfinityMove::ResetParams(int i, int j, bool gold){
    this->row = i, this->col = j; this -> is_gold = (int)gold;
}

// podemos fazer um switch depois
// com Enum
void InfinityMove::DiagonalLeftBottom(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(1, 1, row < 8 && col < 8);
}

void InfinityMove::DiagonalLeftTop(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(1, -1, row < 8 && col >= 0);
}

void InfinityMove::DiagonalRightBottom(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(-1, 1, row >= 0 && col < 8);
}

void InfinityMove::DiagonalRightTop(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(-1, -1, row >= 0 && col >= 0);
}

void InfinityMove::Top(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(-1,0,row >= 0);
}
void InfinityMove::Bottom(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(1,0,row < 8);
}

void InfinityMove::Left(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(0,-1,col >= 0);
}
void InfinityMove::Right(VecMatrixPosition& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(0,1,col < 8);
}