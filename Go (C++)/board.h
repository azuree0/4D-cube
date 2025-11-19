#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <optional>

constexpr int BOARD_SIZE = 19;
constexpr int EMPTY = 0;
constexpr int BLACK = 1;
constexpr int WHITE = 2;

struct Position {
    int x;
    int y;
    
    Position(int x, int y) : x(x), y(y) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

struct PositionHash {
    std::size_t operator()(const Position& pos) const {
        return std::hash<int>()(pos.x) ^ (std::hash<int>()(pos.y) << 1);
    }
};

class Board {
private:
    int grid[BOARD_SIZE][BOARD_SIZE];
    std::optional<Position> ko;
    std::optional<Position> last_move;
    
    std::vector<Position> get_neighbors(int x, int y) const;
    std::vector<std::vector<Position>> capture_groups(int x, int y, int color);
    std::vector<Position> get_group(int x, int y) const;
    bool group_has_liberties(const std::vector<Position>& group) const;
    bool has_liberties(int x, int y) const;

public:
    Board();
    Board(const Board& other);
    Board& operator=(const Board& other);
    
    int get(int x, int y) const;
    void set(int x, int y, int color);
    
    bool is_valid_move(int x, int y, int color) const;
    bool make_move(int x, int y, int color);
    
    std::vector<Position> get_valid_moves(int color) const;
    int get_territory_owner(int x, int y) const;
};

int get_opponent(int color);

#endif // BOARD_H

