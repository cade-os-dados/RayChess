module;

#include <raylib.h>

export module coord;
import <vector>;

/* Matrix position, between 0 and 7 that represents the board */
export struct MatrixPosition{int row, col;};
export struct Coords{ 
    int x, y;
    Vector2 to_vec2(){ return {(float)x, (float)y}; }
};
export struct Resolution{int width, height;}; // screen resolution
export struct CelDim{int width; int height;}; // cel dimensions
export using VecMatrixPosition = std::vector<MatrixPosition>;

export CelDim to_cel_dim(Resolution resolution)
{
    // xadrez 8x8
    return {
        (int)(resolution.width/8), 
        (int)(resolution.height/8)
    };
}

export Coords from_matrix_position(MatrixPosition matrix_pos, CelDim cel)
{
    int x = matrix_pos.col * cel.width;
    int y = matrix_pos.row * cel.height;
    return {x,y};
}

export Coords trunc_coords(Coords coords, CelDim cel)
{
    int x = (int)(coords.x / cel.width) * cel.width;
    int y = (int)(coords.y / cel.height) * cel.height;
    return {x,y};
}

export MatrixPosition from_coords(Coords coords, CelDim cel)
{
    return {
        (int)(coords.y / cel.height),
        (int)(coords.x / cel.width)
    };
}