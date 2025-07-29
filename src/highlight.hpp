#pragma once
#include "pieces.hpp"
#include <memory>

class HighLightControler
{
private:
    bool clicked, on, m_is_gold;
    std::shared_ptr<Peca> m_peca;
    int idx, nclicks;
public:
    HighLightControler(void)
    {
        clicked = false;
        nclicks = 0;
    }
    void Change(bool changer){on = changer;}
    void HighlightedColorIsGold(bool gold) {m_is_gold = gold;}
    bool IsGold(void){return m_is_gold;}
    bool is_on(void){
        return on;
    }
    int DoubleClickedOnPiece(int clickIdx)
    {
        nclicks = clickIdx == idx ? nclicks + 1 : 0;
        return nclicks == 2;
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
    int getPieceIndex(void){return idx;}
    std::shared_ptr<Peca> getPiece(void){return m_peca;};
    void setPiece(std::shared_ptr<Peca> peca){m_peca = peca;}
    void setPieceIndex(int index){idx = index;}
};