#pragma once
#include <raylib.h>
#include <iostream>
#include "move.hpp"
#include "types.hpp"

void DrawResize(Texture2D texture, Rectangle new_size)
{
    DrawTexturePro(
        texture, 
        {0, 0, (float) texture.height, (float) texture.width},
        new_size,
        {0,0}, // origin
        0.0, // rotation
        WHITE
    );
}

Texture2D load_texture(bool gold, const char* path)
{
    Image image = LoadImage(path);
    Color cor = gold ? GOLD : VIOLET;
    ImageColorReplace(&image, BLACK, cor);
    return LoadTextureFromImage(image);
}

Peca::Peca(bool gold, const char* imagePath, float height_if_gold, float height_if_violet, InfinityMove* mv) 
{
    m_height = gold ? height_if_gold : height_if_violet;
    texture = load_texture(gold, imagePath);
    is_gold = gold; move = mv;
}
void Peca::Draw(){
    if(active){
        DrawResize(texture, {m_width, m_height, 100, 75});
    }
}
void Peca::Move(Vector2 new_pos)
{
    m_width = new_pos.x;
    m_height = new_pos.y;
}
void Peca::Move(MatrixPosition new_pos, CelDim cel)
{
    Coords coords = from_matrix_position(new_pos, cel);
    m_width = static_cast<float>(coords.x);
    m_height = static_cast<float>(coords.y);
}
void Peca::Kill(){active = false;}
void Peca::ReSpawn(){
    active = true;
    m_width = OriginalPlace[0];
    m_height = OriginalPlace[1];
}
void Peca::RegisterOriginalPlace(){
    OriginalPlace[0] = m_width;
    OriginalPlace[1] = m_height;
}

class Peao : public Peca {
public:
    Peao(bool gold, int i, InfinityMove* mv) : Peca(gold, "img/Sprite-0001.png", 75, 6*75, mv){
        m_width = (float)100*i; 
        this->RegisterOriginalPlace();
    }
    bool isOnInitialPosition(void)
    {
        return (m_width == OriginalPlace[0] && m_height == OriginalPlace[1]);
    }
    inline void AppendCoordIfAttack(VecCoords& coords, int row, int col)
    {
        std::cout << "[DEBUG] (" << row << "," <<  
            col << "): "
            << this->move->VerifyPosition(row,col,this->is_gold) << std::endl;
        if(this->move->VerifyPosition(row,col,this->is_gold) == Action::attack)
            append_coords(coords,row,col);
    }
    inline void AppendCoordIfMovable(VecCoords& coords, int row, int col)
    {
        if(this->move->VerifyPosition(row,col,this->is_gold) == Action::movable)
            append_coords(coords,row,col);
    }

    VecCoords PossibleMoveCoords(int i, int j) override
    {
        VecCoords coords;
        if(is_gold)
        {
            AppendCoordIfMovable(coords,i+1,j); // down
            AppendCoordIfAttack(coords,i+1,j-1); // diagonal down-left
            AppendCoordIfAttack(coords,i+1,j+1); // diagonal down-right

            // passo duplo
            if(this->isOnInitialPosition() && 
                this->move->VerifyPosition(i+1,j,this->is_gold) == Action::movable &&
                this->move->VerifyPosition(i+2,j,this->is_gold) == Action::movable)
            {
                append_coords(coords,i+2,j);
            }
        }else{
            AppendCoordIfMovable(coords,i-1,j); // up
            AppendCoordIfAttack(coords,i-1,j-1); // diagonal up-left
            AppendCoordIfAttack(coords,i-1,j+1); // diagonal up-right

            // passo duplo
            if(this->isOnInitialPosition() && 
                this->move->VerifyPosition(i-1,j,this->is_gold) == Action::movable &&
                this->move->VerifyPosition(i-2,j,this->is_gold) == Action::movable)
            {
                append_coords(coords,i-2,j);
            }
        }

        return coords;
    }
};

class Cavalo : public Peca {
public:
    Cavalo(bool gold, bool left, InfinityMove* mv) : Peca(gold, "img/Sprite-0002.png", 0, 7*75, mv){
        m_width = left ? 100 : 600;
        this->RegisterOriginalPlace();
    }

    VecCoords PossibleMoveCoords(int i, int j) override
    {
        VecCoords coords;
        
        // right 1
        append_coords(coords,i+1,j+2);
        append_coords(coords,i+1,j-2);
        // right 2
        append_coords(coords,i+2,j+1);
        append_coords(coords,i+2,j-1);
        // left 1
        append_coords(coords,i-1,j+2);
        append_coords(coords,i-1,j-2);
        // left 2
        append_coords(coords,i-2,j+1);
        append_coords(coords,i-2,j-1);

        return coords;
    }
};

class Torre : public Peca {
public:
    Torre(bool gold, bool left, InfinityMove* mv) : Peca(gold, "img/Sprite-0003.png", 0, 7*75, mv){
        m_width = left ? 0 : 700;
        this->RegisterOriginalPlace();
    }

    VecCoords PossibleMoveCoords(int i, int j) override
    {
        VecCoords coords;

        this->move->Left(coords,i,j,this->is_gold);
        this->move->Right(coords,i,j,this->is_gold);
        this->move->Top(coords,i,j,this->is_gold);
        this->move->Bottom(coords,i,j,this->is_gold);

        return coords;
    }
};

class Bispo : public Peca {
public:
    Bispo(bool gold, bool left, InfinityMove* mv) : Peca(gold, "img/Sprite-0004.png", 0, 7*75, mv){ 
        m_width = left ? 200 : 500;
        this->RegisterOriginalPlace();
    }

    VecCoords PossibleMoveCoords(int i, int j) override 
    {
        VecCoords coords;

        this->move->DiagonalLeftBottom(coords,i,j,this->is_gold);
        this->move->DiagonalRightTop(coords,i,j,this->is_gold);
        this->move->DiagonalRightBottom(coords,i,j,this->is_gold);
        this->move->DiagonalLeftTop(coords,i,j,this->is_gold);

        return coords;
    }
};

class Rainha : public Peca {
public:
    Rainha(bool gold, InfinityMove* mv) : Peca(gold, "img/Sprite-0005.png", 0, 7*75, mv){
        m_width = 300;
        this->RegisterOriginalPlace();
    }
    VecCoords PossibleMoveCoords(int i, int j) override 
    {
        VecCoords coords;

        this->move->DiagonalLeftBottom(coords,i,j,this->is_gold);
        this->move->DiagonalRightTop(coords,i,j,this->is_gold);
        this->move->DiagonalRightBottom(coords,i,j,this->is_gold);
        this->move->DiagonalLeftTop(coords,i,j,this->is_gold);
        this->move->Left(coords,i,j,this->is_gold);
        this->move->Right(coords,i,j,this->is_gold);
        this->move->Top(coords,i,j,this->is_gold);
        this->move->Bottom(coords,i,j,this->is_gold);

        return coords;
    }
};

class Rei : public Peca {
public:
    Rei(bool gold, InfinityMove* mv) : Peca(gold, "img/Sprite-0006.png", 0, 7*75, mv){
        m_width = 400;
        this->RegisterOriginalPlace();
    }
    VecCoords PossibleMoveCoords(int i, int j) override
    {
        VecCoords coords;
        append_coords(coords,i,j+1); // up
        append_coords(coords,i,j-1); // down

        // right
        append_coords(coords,i+1,j);
        append_coords(coords,i+1,j+1); // diagonal
        append_coords(coords,i+1,j-1); // diagonal

        // left
        append_coords(coords,i-1,j);
        append_coords(coords,i-1,j+1); // diagonal
        append_coords(coords,i-1,j-1); // diagonal

        return coords;
    }
};

pecas InitPecas(bool is_gold, InfinityMove* move)
{
    pecas pecas;
    pecas.push_back(std::make_shared<Torre>(is_gold, true, move));
    pecas.push_back(std::make_shared<Cavalo>(is_gold, true, move));
    pecas.push_back(std::make_shared<Bispo>(is_gold,true, move)); 
    pecas.push_back(std::make_shared<Rainha>(is_gold, move));
    pecas.push_back(std::make_shared<Rei>(is_gold, move));
    pecas.push_back(std::make_shared<Bispo>(is_gold, false, move));
    pecas.push_back(std::make_shared<Cavalo>(is_gold, false, move));
    pecas.push_back(std::make_shared<Torre>(is_gold, false, move));
    for(int i = 0; i < 8; i++){
        pecas.push_back(std::make_shared<Peao>(is_gold,i,move)); // PEAO
    }
    return pecas;
}