#pragma once 
#include <vector>
#include <memory>
#include <raylib.h>
#include "coord.hpp"

enum PIECE_COLOR{
    PIECE_COLOR_GOLD,
    PIECE_COLOR_VIOLET
};

class InfinityMove{
private:
    Board* m_Board;
    int row, col, is_gold;
public:
    InfinityMove(Board* board) : m_Board(board) {}
    Action VerifyPosition(int k, int l, bool gold);
    bool BreakCondition(int k, int l, int is_gold);
    bool AppendAndCheck(VecMatrixPosition& coords);
    void ResetParams(int i, int j, bool gold);
    void DiagonalLeftBottom(VecMatrixPosition& coords, int i, int j, bool gold);
    void DiagonalLeftTop(VecMatrixPosition& coords, int i, int j, bool gold);
    void DiagonalRightBottom(VecMatrixPosition& coords, int i, int j, bool gold);
    void DiagonalRightTop(VecMatrixPosition& coords, int i, int j, bool gold);
    void Top(VecMatrixPosition& coords, int i, int j, bool gold);
    void Bottom(VecMatrixPosition& coords, int i, int j, bool gold);
    void Left(VecMatrixPosition& coords, int i, int j, bool gold);
    void Right(VecMatrixPosition&, int, int, bool);
};

class Peca {
protected:
    Texture2D texture;
    size_t OriginalPlace[2];
    float m_width, m_height;
    bool is_gold;
    InfinityMove* move;
    bool active = true;
public:
    Peca(bool gold, const char* imagePath, float height_if_gold, float height_if_violet, InfinityMove* mv);
    virtual ~Peca() { UnloadTexture(texture); }
    void Draw();
    virtual VecMatrixPosition PossibleMoveCoords(int i, int j) = 0;
    std::tuple<int,int> coords(void){return std::make_tuple(m_width, m_height);}
    Coords coords(bool placeholder){return {(int)m_width, (int)m_height};}
    void Move(Vector2 new_pos);
    void Move(MatrixPosition, CelDim);
    void Kill();
    void ReSpawn();
    void RegisterOriginalPlace();
};

typedef std::vector<std::shared_ptr<Peca>> pecas;
