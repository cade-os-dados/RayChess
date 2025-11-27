#include "../src/parser.hpp"

int main() {
    std::string input = "Move: 4 -> (5,6)";
    std::cmatch match;
    if(!is_sync_move(input, match)) throw std::runtime_error("Erro no regex");
    SyncMove sync = parse(input);

    assert(sync.piece == 4);
    assert(sync.x == 5);
    assert(sync.y == 6);

    SyncMove sync2 = parse_with_regex(match);

    assert(sync2.piece == 4);
    assert(sync2.x == 5);
    assert(sync2.y == 6);

    SyncMove case2 = parse(to_str({14,0,0}));
    assert(case2.piece == 14 && case2.x == 0 && case2.y == 0);
    assert(to_str({14,0,0}) == "Move: 14 -> (0,0)");

    std::cout << "OK" << std::endl;
}