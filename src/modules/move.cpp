import coord;
import move;
import board;
import <iostream>;
// import <vector>;

const int W = 800;
const int H = 600;

int main(void)
{
    int n = 8;
    int cel_width = (int) (W / n); 
    int cel_height = (int) (H / n); 

    Board board(cel_width, cel_height, n);
    InfinityMove move(&board);

    VecMatrixPosition coords;

    move.Left(coords,0,4,true);

    for(MatrixPosition pos : coords)
    {
        std::cout  << "Row: " << pos.row << " Col: " << pos.col << std::endl;
    }
}