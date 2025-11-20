#pragma once
#include "pieces.hpp"
#include <memory>
#include "types.hpp"

// typedef std::vector<std::shared_ptr<Peca>> pecas;

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
    void Activate(){on=true;}
    void Deactivate(){on=false;}
    void Change(bool on_){on = on_;}
    bool IsGold(void){return this->idx > 0;}
    bool is_on(void){return on;}
    bool to_change(int where_clicked, bool is_gold_turn)
    {
        if(on) return false;
        bool is_gold = where_clicked > 0;
        bool is_violet = where_clicked < 0;
        return (is_gold_turn && is_gold) 
            || (!is_gold_turn && is_violet);
    }

    void setHightlight(
        int where_clicked, 
        bool is_gold_turn,
        VecCoords& cache_possible_moves,
        Board& board
    ){
        if(on) return; // already on - ignore
        bool highlight_gold = where_clicked > 0 && is_gold_turn;
        bool highlight_violet = where_clicked < 0 && !is_gold_turn;
        if(!highlight_gold && !highlight_violet) return; // nada a alterar
        
        this -> setPieceIndex(where_clicked); // altera o índice da peça highlightada
        auto piece = this -> getPiece();
        auto [i,j] = board.from_coord(piece -> coords()); // pega os indices da peça em termos de i,j
        cache_possible_moves = piece -> PossibleMoveCoords(i,j); // depois podemos modificar para gerar essas coordenadas diretamente pela api da peça

        if(!cache_possible_moves.empty())
        {
             this -> Activate();
             board.Highlight(cache_possible_moves, is_gold_turn);
        }
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
    VecCoords GetMoves(int i, int j)
    {
        return this -> getPiece() -> PossibleMoveCoords(i,j);
    }

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