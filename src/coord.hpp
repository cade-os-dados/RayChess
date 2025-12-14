#pragma once
#include <raylib.h>
#include <vector>
#include <algorithm>

/* Matrix position, between 0 and 7 that represents the board */
struct MatrixPosition{
    int row, col;
    bool operator==(const MatrixPosition& other) const
    {
        return row == other.row && col == other.col;
    }
};
struct Coords{ 
    int x, y;
    Vector2 to_vec2(){ return {(float)x, (float)y}; }
};
struct Resolution{int width, height;}; // screen resolution
struct CelDim{int width; int height;}; // cel dimensions
typedef std::vector<MatrixPosition> VecMatrixPosition;

CelDim to_cel_dim(Resolution resolution)
{
    // xadrez 8x8
    return {
        (int)(resolution.width/8), 
        (int)(resolution.height/8)
    };
}

Coords from_matrix_position(MatrixPosition matrix_pos, CelDim cel)
{
    int x = matrix_pos.col * cel.width;
    int y = matrix_pos.row * cel.height;
    return {x,y};
}

Coords trunc_coords(Coords coords, CelDim cel)
{
    int x = (int)(coords.x / cel.width) * cel.width;
    int y = (int)(coords.y / cel.height) * cel.height;
    return {x,y};
}

MatrixPosition from_coords(Coords coords, CelDim cel)
{
    return {
        (int)(coords.y / cel.height),
        (int)(coords.x / cel.width)
    };
}

bool ContainsMatrixPos(const VecMatrixPosition& vec, MatrixPosition pos)
{
    return std::count(vec.begin(),vec.end(), pos); // se 0 false, se >= 1 true
}