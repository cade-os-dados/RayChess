#pragma once
#include <raylib.h>

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