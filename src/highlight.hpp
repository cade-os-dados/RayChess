class HighLightControler
{
private:
    bool last_click, clicked, on;
public:
    HighLightControler(void)
    {
        last_click = false;
        clicked = false;
    }
    void Change(bool changer){on = changer;}
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