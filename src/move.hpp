#pragma once
#include <vector>
#include "board.hpp"
#include "coord.hpp"
#include "types.hpp"
#include <functional>

#define LOWER_BOARD_BOUND 0
#define UPPER_BOARD_BOUND 8

void append_coords(VecMatrixPosition& vec, int i, int j){
    vec.push_back({i,j});
}

struct Increment {
    int row_pace;
    int col_pace;
};

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

bool in(DIRECTION direction, std::initializer_list<DIRECTION> const &directions)
{
    return std::find(directions.begin(), directions.end(), direction) != directions.end();
}

void set_row_pace(Increment& increment, DIRECTION direction)
{
    if(in(direction,{LEFT,DIAGONAL_LEFT_BOTTOM, DIAGONAL_LEFT_TOP}))
        increment.row_pace = -1; // LEFT
    else if(in(direction,{RIGHT, DIAGONAL_RIGHT_BOTTOM, DIAGONAL_RIGHT_TOP}))
        increment.row_pace = 1; // RIGHT
    else
        increment.row_pace = 0;
}

void set_col_pace(Increment& increment, DIRECTION direction)
{
    if(in(direction,{TOP, DIAGONAL_LEFT_TOP, DIAGONAL_RIGHT_TOP}))
        increment.col_pace = -1; //TOP
    else if(in(direction,{BOTTOM, DIAGONAL_LEFT_BOTTOM, DIAGONAL_RIGHT_BOTTOM}))
        increment.col_pace = 1; // BOTTOM
    else
        increment.col_pace = 0;
}

bool check_board_bounds(int pace, int value)
{
    switch(pace)
    {
        case 0: return true;
        case 1: return value < UPPER_BOARD_BOUND;
        case -1: return value >= LOWER_BOARD_BOUND;
    }
    return true; // apenas para o compilador não reclamar
}

bool check_board_bounds(Increment increment, int row, int col)
{
    bool row_condition = check_board_bounds(increment.row_pace, row);
    bool col_condition = check_board_bounds(increment.col_pace, col);
    return row_condition && col_condition;
}

void InfinityMove::SetMovement(VecMatrixPosition& coords, 
    MatrixPosition actual_pos, bool gold, DIRECTION direction)
{
    Increment increment;
    set_row_pace(increment, direction);
    set_col_pace(increment, direction);

    this->ResetParams(actual_pos.row, actual_pos.col, gold);

    while(check_board_bounds(increment, this -> row, this -> col)){
        this -> row += increment.row_pace; 
        this -> col += increment.col_pace;
        if(this->AppendAndCheck(coords))
            break;
    }
}

VecMatrixPosition possible_infinity_movements(InfinityMove* move, 
    MatrixPosition actual_pos, 
    bool is_gold, 
    std::initializer_list<DIRECTION> directions)
{
    VecMatrixPosition coords;
    for(auto direction : directions)
        move -> SetMovement(coords,actual_pos,is_gold,direction);
    return coords;
}

VecMatrixPosition possible_movements( 
    MatrixPosition actual_pos, 
    std::initializer_list<MatrixPosition> offsets)
{
    VecMatrixPosition coords;
    for(auto offset : offsets)
        coords.push_back(actual_pos+offset);
    return coords;
}