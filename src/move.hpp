#pragma once
#include <vector>
#include "board.hpp"
#include "types.hpp"

#define INCREMENT_UNTIL(incRow,incCol,cond) \
    this->ResetParams(i,j,gold);\ 
    while(cond){\
    row+=incRow; col+=incCol;\
    if(this->AppendAndCheck(coords))\
        break;\
    }

void append_coords(VecCoords& vec, int i, int j){
    vec.push_back(std::make_tuple(i,j));
}

Action InfinityMove::VerifyPosition(int k, int l, bool gold){
    return m_Board -> VerifyPosition(k,l,(int)gold);
}
bool InfinityMove::BreakCondition(int k, int l, int is_gold){
    return m_Board -> VerifyPosition(k,l,is_gold) != Action::movable;
}
bool InfinityMove::AppendAndCheck(VecCoords& coords){
    append_coords(coords,row,col);
    return this->BreakCondition(row,col,this->is_gold);
}
void InfinityMove::ResetParams(int i, int j, bool gold){
    this->row = i, this->col = j; this -> is_gold = (int)gold;
}

// podemos fazer um switch depois
// com Enum
void InfinityMove::DiagonalLeftBottom(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(1, 1, row < 8 && col < 8);
}

void InfinityMove::DiagonalLeftTop(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(1, -1, row < 8 && col >= 0);
}

void InfinityMove::DiagonalRightBottom(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(-1, 1, row >= 0 && col < 8);
}

void InfinityMove::DiagonalRightTop(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(-1, -1, row >= 0 && col >= 0);
}

void InfinityMove::Top(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(-1,0,row >= 0);
}
void InfinityMove::Bottom(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(1,0,row < 8);
}

void InfinityMove::Left(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(0,-1,col >= 0);
}
void InfinityMove::Right(VecCoords& coords, int i, int j, bool gold){
    INCREMENT_UNTIL(0,1,col < 8);
}