class HighLightControler
{
private:
    bool last_click, clicked, on, m_is_gold;
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
};