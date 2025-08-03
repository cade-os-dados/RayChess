#pragma once
#include "pieces.hpp"
#include <memory>

typedef std::vector<std::shared_ptr<Peca>> pecas;

class HighLightControler
{
private:
    bool clicked, on, rehighlight;
    pecas *violet, *gold;
    int idx, nclicks;
public:
    void SetPieces(pecas *pecas_, bool gold){
        if(gold){
            this->gold = pecas_;
        }else{
            this->violet = pecas_;
        }
    }
    HighLightControler(void)
    {
        clicked = false;
        nclicks = 0;
        rehighlight = false;
    }
    void Change(bool changer){on = changer;}
    bool IsGold(void){return this->idx > 0;}
    bool is_on(void){
        return on;
    }

    int GetNClicks(){
        return nclicks;
    }
    void UpdateClicked(bool clicou)
    {
        clicked = clicou;
        if(clicou == false)
            nclicks = 0;
    }
    bool Unhighlight(void){
        return !clicked;
    }
    bool ReHighlight(void){
        return rehighlight;
    }
    int getPieceIndex(void){return idx;}
    std::shared_ptr<Peca> getPiece(void){
        if (idx == 0) return nullptr;
        else if (idx > 0){
            return (*this->gold)[idx-1];
        }
        else if (idx < 0){
            return (*this->violet)[abs(idx)-1];
        }
    }

    void setPieceIndex(int index){idx = index;}
    void setReHighlight(bool value){rehighlight = value;}

    // conditions
    int DoubleClickedOnPiece(int clickIdx)
    {
        nclicks = clickIdx == idx ? nclicks + 1 : 0;
        return nclicks == 2;
    }
    bool ClickedSameTeam(int clickIdx){
        bool gold_clicked_gold = this->IsGold() && clickIdx > 0;
        bool violet_clicked_violet = !this->IsGold() && clickIdx < 0;
        return gold_clicked_gold || violet_clicked_violet;
    }

};