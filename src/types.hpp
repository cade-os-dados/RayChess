#pragma once 
#include <vector>
#include <memory>
#include <raylib.h>
#include "coord.hpp"

enum NETWORK_SIDE 
{
    SERVER_SIDE,
    CLIENT_SIDE
};

enum PIECE_COLOR{
    PIECE_COLOR_GOLD,
    PIECE_COLOR_VIOLET
};

enum DIRECTION{
    DIAGONAL_LEFT_BOTTOM,
    DIAGONAL_LEFT_TOP,
    DIAGONAL_RIGHT_BOTTOM,
    DIAGONAL_RIGHT_TOP,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
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
    void SetMovement(VecMatrixPosition& coords, MatrixPosition actual_pos, bool gold, DIRECTION direction);
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
