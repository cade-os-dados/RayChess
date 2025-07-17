#pragma once
#include <raylib.h>
#include <iostream>

typedef std::vector<std::tuple<int,int>> VecCoords;

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

void append_coords(VecCoords& vec, int i, int j){
    vec.push_back(std::make_tuple(i,j));
}

class Peca {
protected:
    Texture2D texture;
    float m_width, m_height;
public:
    Peca(bool gold, const char* imagePath, float height_if_gold, float height_if_violet) 
    {
        m_height = gold ? height_if_gold : height_if_violet;
        texture = load_texture(gold, imagePath);
    }
    virtual ~Peca() { UnloadTexture(texture); }
    virtual void Draw() = 0; // Método virtual puro para obrigar as subclasses a implementar
    virtual VecCoords PossibleMoveCoords(int i, int j) = 0;
    std::tuple<int,int> coords(void){return std::make_tuple(m_width, m_height);}
    void Move(Vector2 new_pos)
    {
        m_width = new_pos.x;
        m_height = new_pos.y;
    }
};

class Peao : public Peca {
public:
    Peao(bool gold, int i) : Peca(gold, "img/Sprite-0001.png", 75, 6*75){m_width = (float)100*i;}
    void Draw() override
    {
        // for(int i = 0; i < 8; i++)
        //     DrawResize(texture, {(float) 100*i, m_height, 100, 75});
        DrawResize(texture,{m_width,m_height,100,75});
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

class Cavalo : public Peca {
public:
    Cavalo(bool gold, bool left) : Peca(gold, "img/Sprite-0002.png", 0, 7*75){
        m_width = left ? 100 : 600;
    }
    void Draw() override
    {
        DrawResize(texture, {m_width, m_height, 100, 75});
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
    Torre(bool gold, bool left) : Peca(gold, "img/Sprite-0003.png", 0, 7*75){m_width = left ? 0 : 700;}
    void Draw() override
    {
        // DrawResize(texture, {0, m_height, 100, 75});
        // DrawResize(texture, {700, m_height, 100, 75});
        DrawResize(texture, {m_width, m_height, 100, 75});
    }

    VecCoords PossibleMoveCoords(int i, int j) override
    {
        VecCoords coords;

        // left/right/up/down
        for(int p = 0; p < 8; p++){
            if(p != i)
                append_coords(coords,p,j);
            if(p!= j)
                append_coords(coords,i,p);
        }

        return coords;
    }
};

class Bispo : public Peca {
public:
    Bispo(bool gold, bool left) : Peca(gold, "img/Sprite-0004.png", 0, 7*75){ m_width = left ? 200 : 500;}
    void Draw() override
    {
        // DrawResize(texture, {200, m_height, 100, 75});
        // DrawResize(texture, {500, m_height, 100, 75});
        DrawResize(texture, {m_width, m_height, 100, 75});
    }

    VecCoords PossibleMoveCoords(int i, int j) override 
    {
        VecCoords coords;

        // Diagonal -> LeftDown
        int k = i, l = j;
        while(k < 8 && l < 8){
            k++; l++;
            append_coords(coords,k,l);
        }

        // Diagonal -> RightUp
        k = i, l = j;
        while(k >= 0 && l >= 0){
            k--; l--;
            append_coords(coords,k,l);
        }

        // Diagonal -> RightDown
        k = i, l = j;
        while(k >= 0 && l < 8){
            k--; l++;
            append_coords(coords,k,l);
        }

        // Diagonal -> LeftUp
        k = i, l = j;
        while(k < 8 && l >= 0){
            k++; l--;
            append_coords(coords,k,l);
        }

        return coords;
    }
};

class Rainha : public Peca {
public:
    Rainha(bool gold) : Peca(gold, "img/Sprite-0005.png", 0, 7*75){m_width = 300;}
    void Draw() override
    {
        DrawResize(texture, {m_width, m_height, 100, 75});
    }
    VecCoords PossibleMoveCoords(int i, int j) override 
    {
        VecCoords coords;

        // Diagonal -> LeftDown
        int k = i, l = j;
        while(k < 8 && l < 8){
            k++; l++;
            append_coords(coords,k,l);
        }

        // Diagonal -> RightUp
        k = i, l = j;
        while(k >= 0 && l >= 0){
            k--; l--;
            append_coords(coords,k,l);
        }

        // Diagonal -> RightDown
        k = i, l = j;
        while(k >= 0 && l < 8){
            k--; l++;
            append_coords(coords,k,l);
        }

        // Diagonal -> LeftUp
        k = i, l = j;
        while(k < 8 && l >= 0){
            k++; l--;
            append_coords(coords,k,l);
        }

        // left/right/up/down
        for(int p = 0; p < 8; p++){
            if(p != i)
                append_coords(coords,p,j);
            if(p!= j)
                append_coords(coords,i,p);
        }

        return coords;
    }
};

class Rei : public Peca {
public:
    Rei(bool gold) : Peca(gold, "img/Sprite-0006.png", 0, 7*75){m_width = 400;}
    void Draw() override
    {
        DrawResize(texture, {m_width, m_height, 100, 75});
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