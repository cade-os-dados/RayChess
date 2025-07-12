#pragma once
#include <raylib.h>
#include <iostream>

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
    std::tuple<int,int> coords(void){return std::make_tuple(m_width, m_height);}
};

class Peao : public Peca {
public:
    Peao(bool gold) : Peca(gold, "img/Sprite-0001.png", 75, 6*75){}
    void Draw() override
    {
        for(int i = 0; i < 8; i++)
            DrawResize(texture, {(float) 100*i, m_height, 100, 75});
    }
};

class Cavalo : public Peca {
public:
    Cavalo(bool gold) : Peca(gold, "img/Sprite-0002.png", 0, 7*75){}
    void Draw() override
    {
        DrawResize(texture, {100, m_height, 100, 75});
        DrawResize(texture, {600, m_height, 100, 75});
    }
};

class UniqueCavalo : public Peca {
public:
    UniqueCavalo(bool gold, bool left) : Peca(gold, "img/Sprite-0002.png", 0, 7*75){
        m_width = left ? 100 : 600;
    }
    void Draw() override
    {
        DrawResize(texture, {m_width, m_height, 100, 75});
    }
    void Move(Vector2 new_pos)
    {
        m_width = new_pos.x;
        m_height = new_pos.y;
    }
    void MoveOnClick()
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePosition = GetMousePosition();
            std::cout << "[Debug] - Position - X: " << mousePosition.x << " Y: " << mousePosition.y << std::endl;
            // Handle the click event here
            m_width = (int)(mousePosition.x / 100) * 100;
            m_height = (int)(mousePosition.y / 75) * 75;
        }
    }

    std::vector<std::tuple<int,int>> movableCoords(int i, int j){
        std::vector<std::tuple<int,int>> coords;
        
        // right 1
        coords.push_back(std::make_tuple(i+1,j+2));
        coords.push_back(std::make_tuple(i+1,j-2));
        // right 2
        coords.push_back(std::make_tuple(i+2,j+1));
        coords.push_back(std::make_tuple(i+2,j-1));
        // left 1
        coords.push_back(std::make_tuple(i-1,j+2));
        coords.push_back(std::make_tuple(i-1,j-2));
        // left 2
        coords.push_back(std::make_tuple(i-2,j+1));
        coords.push_back(std::make_tuple(i-2,j-1));

        return coords;
    }
};

class Torre : public Peca {
public:
    Torre(bool gold) : Peca(gold, "img/Sprite-0003.png", 0, 7*75){}
    void Draw() override
    {
        DrawResize(texture, {0, m_height, 100, 75});
        DrawResize(texture, {700, m_height, 100, 75});
    }
};

class Bispo : public Peca {
public:
    Bispo(bool gold) : Peca(gold, "img/Sprite-0004.png", 0, 7*75){}
    void Draw() override
    {
        DrawResize(texture, {200, m_height, 100, 75});
        DrawResize(texture, {500, m_height, 100, 75});
    }
};

class Rainha : public Peca {
public:
    Rainha(bool gold) : Peca(gold, "img/Sprite-0005.png", 0, 7*75){}
    void Draw() override
    {
        DrawResize(texture, {300, m_height, 100, 75});
    }
};

class Rei : public Peca {
public:
    Rei(bool gold) : Peca(gold, "img/Sprite-0006.png", 0, 7*75){}
    void Draw() override
    {
        DrawResize(texture, {400, m_height, 100, 75});
    }
};