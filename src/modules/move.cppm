module;

#include "raylib.h"

export module move;
import coord;
import board;
import <vector>;

#define INCREMENT_UNTIL(incRow,incCol,cond)\
    this->ResetParams(i,j,gold);\
    while(cond){\
    row+=incRow; col+=incCol;\
    if(this->AppendAndCheck(coords))\
        break;\
    }

void append_coords(VecMatrixPosition& vec, int i, int j){
    vec.push_back({i,j});
}

export class InfinityMove{
private:
    Board* m_Board;
    int row, col, is_gold;
public:
    InfinityMove(Board* board) : m_Board(board) {}
    Action VerifyPosition(int k, int l, bool gold){
        return m_Board -> VerifyPosition(k,l,(int)gold);
    };
    bool BreakCondition(int k, int l, int is_gold){
        return m_Board -> VerifyPosition(k,l,is_gold) != Action::movable;
    };
    bool AppendAndCheck(VecMatrixPosition& coords){
        append_coords(coords,row,col);
        return this->BreakCondition(row,col,this->is_gold);
    };
    void ResetParams(int i, int j, bool gold){
        this->row = i, this->col = j; this -> is_gold = (int)gold;
    };

    // transformar em um enum: DiagonalLeftBottom/DiagionalLeftTop/DiagonalRightBottom, etc...

    void DiagonalLeftBottom(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(1, 1, row < 8 && col < 8);
    };
    void DiagonalLeftTop(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(1, -1, row < 8 && col >= 0);
    };
    void DiagonalRightBottom(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(-1, 1, row >= 0 && col < 8);
    };
    void DiagonalRightTop(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(-1, -1, row >= 0 && col >= 0);
    };
    void Top(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(-1,0,row >= 0);
    };
    void Bottom(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(1,0,row < 8);
    };
    void Left(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(0,-1,col >= 0);
    };
    void Right(VecMatrixPosition& coords, int i, int j, bool gold){
        INCREMENT_UNTIL(0,1,col < 8);
    };
};