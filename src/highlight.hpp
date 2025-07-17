#pragma once
#include "pieces.hpp"
#include <memory>

class HighLightControler
{
private:
    bool last_click, clicked, on, m_is_gold;
    std::shared_ptr<Peca> m_peca;
    int idx;
public:
    HighLightControler(void)
    {
        last_click = false;
        clicked = false;
    }
    void Change(bool changer){on = changer;}
    void HighlightedColorIsGold(bool gold) {m_is_gold = gold;}
    bool is_gold(void){return m_is_gold;}
    bool is_on(void){
        return on;
    }
    void UpdateClicked(bool clicou)
    {
        last_click = clicked;
        clicked = clicou;
    }
    bool Unhighlight(void){return clicked == false;}
    int getPieceIndex(void){return idx;}
    std::shared_ptr<Peca> getPiece(void){return m_peca;};
    void setPiece(std::shared_ptr<Peca> peca){m_peca = peca;}
    void setPieceIndex(int index){idx = index;}
};