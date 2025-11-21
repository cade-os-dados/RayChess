#pragma once 
#include <vector>
#include <memory>
#include <raylib.h>

typedef std::vector<std::tuple<int,int> > VecCoords;

class InfinityMove{
private:
    Board* m_Board;
    int row, col, is_gold;
public:
    InfinityMove(Board* board) : m_Board(board) {}
    Action VerifyPosition(int k, int l, bool gold);
    bool BreakCondition(int k, int l, int is_gold);
    bool AppendAndCheck(VecCoords& coords);
    void ResetParams(int i, int j, bool gold);
    void DiagonalLeftBottom(VecCoords& coords, int i, int j, bool gold);
    void DiagonalLeftTop(VecCoords& coords, int i, int j, bool gold);
    void DiagonalRightBottom(VecCoords& coords, int i, int j, bool gold);
    void DiagonalRightTop(VecCoords& coords, int i, int j, bool gold);
    void Top(VecCoords& coords, int i, int j, bool gold);
    void Bottom(VecCoords& coords, int i, int j, bool gold);
    void Left(VecCoords& coords, int i, int j, bool gold);
    void Right(VecCoords&, int, int, bool);
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
    virtual VecCoords PossibleMoveCoords(int i, int j) = 0;
    std::tuple<int,int> coords(void){return std::make_tuple(m_width, m_height);}
    void Move(Vector2 new_pos);
    void Kill();
    void ReSpawn();
    void RegisterOriginalPlace();
};

typedef std::vector<std::shared_ptr<Peca>> pecas;
