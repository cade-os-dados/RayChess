#include <string>
#include <regex>
#include <iostream>
#include <charconv>
#include <cassert>

/*
    Aqui teremos apenas 
    move, <- unica feature implementada
    roque,
    selecao de peça,
    reset (implícito) se já estiver sincronizado
*/

struct SyncMove {
    int piece;
    int x;
    int y;
};

bool is_sync_move(std::string_view string, std::cmatch& match)
{
    static const std::regex regex(R"(Move:\s*(-?\d+)\s*->\s*\((\d+),\s*(\d+)\))");
    return std::regex_match(string.begin(),string.end(), match, regex);
}

bool is_sync_move(std::string_view string)
{
    static const std::regex regex(R"(Move:\s*(-?\d+)\s*->\s*\((\d+),\s*(\d+)\))");
    return std::regex_match(string.begin(), string.end(), regex);
}

inline void set_value(std::string_view string, int& value)
{
    std::from_chars(string.begin(), string.end(), value);
}

inline int find(std::string_view string, int idx, char stopper)
{
    while(string[idx] != stopper) idx++;
    return idx;
}

// supoe que is_sync_move retornou true
SyncMove parse(std::string_view string)
{
    int idx;
    SyncMove sync;

    idx = find(string,6,' ');
    set_value(string.substr(6,idx), sync.piece);

    int offset = idx-7;
    sync.x = string[12+offset] - '0';
    sync.y = string[14+offset] - '0';

    return sync;
}

SyncMove parse_with_regex(std::cmatch match)
{
    SyncMove sync;
    sync.piece = std::stoi(match[1].str());
    sync.x = std::stoi(match[2].str());
    sync.y = std::stoi(match[3].str());
    return sync;
}

std::string to_str(SyncMove sync)
{
    return "Move: " 
        + std::to_string(sync.piece) 
        + " -> (" 
        + std::to_string(sync.x) 
        + ","
        + std::to_string(sync.y)
        + ")";
}
