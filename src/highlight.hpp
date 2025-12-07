#pragma once
#include "pieces.hpp"
#include <memory>
#include "types.hpp"
#include "coord.hpp"

// typedef std::vector<std::shared_ptr<Peca>> pecas;

class HighLightControler
{
private:
    bool clicked{false}, on{false};
    pecas *violet, *gold;
    Board* board;
    int idx, nclicks{0};
public:
    void setPieceIndex(int index){idx = index;}
    void SetBoardPtr(Board* board_ptr){board = board_ptr;}
    void SetPieces(pecas *pecas_, bool gold){
        if(gold) this->gold = pecas_;
        else this->violet = pecas_;
    }

    // control if highlight is on
    void Activate(){on=true;}
    void Deactivate(){on=false;}
    void Change(bool on_){on = on_;}

    // getters
    bool is_on(void){return on;}
    bool IsGold(void){return this->idx > 0;}
    int GetNClicks(){return nclicks;}
    VecMatrixPosition GetMoves(int i, int j){return getPiece() -> PossibleMoveCoords(i,j);}
    int getPieceIndex(void){return idx;}
    std::shared_ptr<Peca> getPiece(void){
        if (idx == 0) return nullptr;
        else if (idx > 0) return (*this->gold)[idx-1];
        else if (idx < 0) return (*this->violet)[abs(idx)-1];
    }
    std::tuple<std::shared_ptr<Peca>, int, bool> GetInfo(){
        return std::make_tuple(getPiece(), getPieceIndex(), IsGold());
    }

    // feature functions
    void setBoardColor(  
        int where_clicked, 
        bool is_gold_turn,
        VecMatrixPosition& cache_possible_moves
    ){
        this -> setPieceIndex(where_clicked); // altera o índice da peça highlightada
        auto piece = this -> getPiece();
        auto [i,j] = board -> from_coord(piece -> coords()); // pega os indices da peça em termos de i,j
        cache_possible_moves = piece -> PossibleMoveCoords(i,j); // depois podemos modificar para gerar essas coordenadas diretamente pela api da peça

        if(cache_possible_moves.empty())
        {
            this -> Deactivate();
            return;
        }

        this -> Activate();
        board -> Highlight(cache_possible_moves, is_gold_turn);
    }

    void setHighlight(
        int where_clicked, 
        bool is_gold_turn,
        VecMatrixPosition& cache_possible_moves
    ){
        if(on) return; // already on - ignore
        bool highlight_gold = where_clicked > 0 && is_gold_turn;
        bool highlight_violet = where_clicked < 0 && !is_gold_turn;
        if(!highlight_gold && !highlight_violet) return; // nada a alterar
        
        this -> setBoardColor(where_clicked, is_gold_turn, cache_possible_moves);
    }


    void Unhighlight(void){
        if(!clicked)
        {
            board -> backupAllCellColor();
            on = false;
        }
    }
    void ReHighlight(        
        int where_clicked, 
        bool is_gold_turn,
        VecMatrixPosition& cache_possible_moves)
    {
        this -> Activate();
        board -> backupAllCellColor();
        this -> setBoardColor(where_clicked,is_gold_turn,cache_possible_moves);
    }
    bool CheckReHighlight(int where_clicked)
    {
        bool condition = !DoubleClickedOnPiece(where_clicked) &&
            ClickedSameTeam(where_clicked);

        if(condition) return true;
        else
        {
            if(false) // debug
            {
                std::cout << "Double Clicked?: " << DoubleClickedOnPiece(where_clicked) << std::endl;
                std::cout << "ClickedSameTeam?: " << ClickedSameTeam(where_clicked) << std::endl;
            }
            this -> UpdateClicked(false);
            this -> Unhighlight();
            return false;
        }
    }

    // control functions
    void UpdateClicked(bool clicou)
    {
        clicked = clicou;
        if(clicou == false) nclicks = 0;
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