#include <iostream>
#include <raylib.h>
#include <vector>
#include <memory>
#include "highlight.hpp"
#include "types.hpp"

class GamePoints{
private:
    int violet, gold;
public:
    GamePoints(){
        violet = 0; gold = 0;
    }
    const char* GoldPoints(){
        return TextFormat("%d PONTOS", gold);
    }
    const char* VioletPoints(){
        return TextFormat("%d PONTOS", violet);
    }
    void AddViolet(){
        violet++;
    }
    void AddGold(){
        gold++;
    }
};

class Game : public GamePoints
{
private:
    Board* board;
    Vector2 pos{0.0f,0.0f};  // mouse truncated pos
    int matrix_pos_x, matrix_pos_y;
    pecas *gold_ptr, *violet_ptr;
    bool* is_gold_turn;
public:
    Game(pecas* gold_, pecas* violet_, bool* is_gold, Board* board_) 
        : gold_ptr(gold_), violet_ptr(violet_), is_gold_turn(is_gold),
        board(board_) {}
    void GetTruncatedPos(Vector2 mousePos)
    {
        auto [x_, y_] = board -> trunc_coord(mousePos.x, mousePos.y);
        pos.x = (float)x_; pos.y = (float)y_;
    }
    void GetMatrixPos(Vector2 mousePos)
    {
        auto [m_x, m_y] = board -> from_coord(mousePos.x, mousePos.y);
        matrix_pos_x = m_x;
        matrix_pos_y = m_y;
    }

    void Reset()
    {
        printf("Changing to gold turn\n");
        *is_gold_turn = true;
        printf("Reseting board\n");
        board -> ResetPositions();
        printf("Respawning pieces\n");
        for(const auto& piece : *gold_ptr) piece -> ReSpawn();
        for(const auto& piece : *violet_ptr) piece -> ReSpawn();
    }

    bool CheckEndGame(int piece, bool is_gold_piece)
    {
        // rei derrotado
        if(piece == 5 && !is_gold_piece)
        {
            this -> AddViolet();
            return true;
        }
            
        else if(piece == -5 && is_gold_piece)
        {
            this -> AddGold();
            return true;
        }
        // else {*is_gold_turn = !(*is_gold_turn); return false; }
        else return false;
    }

    bool CanMove(MatrixPosition pos)
    {
        return matrix_pos_x == pos.row && matrix_pos_y == pos.col;
    }

    auto get_piece_by_idx(int idx, bool is_gold)
    {
        if(is_gold) return (*gold_ptr)[idx];
        else return (*violet_ptr)[idx];
    }

    int Kill(MatrixPosition pos, bool is_gold_piece)
    {
        int piece = board -> Where(pos);
        if(piece < 0 && is_gold_piece)
            get_piece_by_idx(abs(piece)-1, false) -> Kill();
        else if(piece > 0 && !is_gold_piece)
            get_piece_by_idx(piece-1,true) -> Kill();
        return piece;
    }

    /*
        Sistema de movimento tem que ser uma classe a parte!
        Não game!
    */
    void Move(int piece_idx, MatrixPosition new_pos, CelDim cel)
    {
        if(piece_idx == 0) return; // impossible...
        bool is_gold = piece_idx > 0;
        auto piece = get_piece_by_idx(abs(piece_idx)-1,is_gold);

        // clean last position
        MatrixPosition piece_pos = from_coords(piece->coords(true), cel);
        board -> CleanPosition(piece_pos);

        // move and register
        piece -> Move(new_pos,cel);
        board -> RegisterPosition(new_pos,piece_idx);
    }

    void Move(std::shared_ptr<Peca>& piece_ptr, int piece_idx)
    {
        // remove a peça de onde está
        auto [x, y] = board -> from_coord(piece_ptr -> coords());
        board -> CleanPosition(x,y);

        // insere na nova posição
        piece_ptr -> Move(pos); // altera na classe
        board -> RegisterPosition(matrix_pos_x, matrix_pos_y,piece_idx); // altera no board
    }
};