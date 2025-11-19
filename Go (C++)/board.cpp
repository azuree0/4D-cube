#include "board.h"
#include <unordered_set>
#include <algorithm>

Board::Board() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            grid[y][x] = EMPTY;
        }
    }
    ko = std::nullopt;
    last_move = std::nullopt;
}

Board::Board(const Board& other) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            grid[y][x] = other.grid[y][x];
        }
    }
    ko = other.ko;
    last_move = other.last_move;
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                grid[y][x] = other.grid[y][x];
            }
        }
        ko = other.ko;
        last_move = other.last_move;
    }
    return *this;
}

int Board::get(int x, int y) const {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return -1;
    }
    return grid[y][x];
}

void Board::set(int x, int y, int color) {
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        grid[y][x] = color;
    }
}

std::vector<Position> Board::get_neighbors(int x, int y) const {
    std::vector<Position> neighbors;
    if (x > 0) neighbors.push_back(Position(x - 1, y));
    if (x < BOARD_SIZE - 1) neighbors.push_back(Position(x + 1, y));
    if (y > 0) neighbors.push_back(Position(x, y - 1));
    if (y < BOARD_SIZE - 1) neighbors.push_back(Position(x, y + 1));
    return neighbors;
}

std::vector<Position> Board::get_group(int x, int y) const {
    int color = get(x, y);
    if (color == EMPTY || color == -1) {
        return {};
    }
    
    std::vector<Position> group;
    std::unordered_set<Position, PositionHash> visited;
    std::vector<Position> stack;
    stack.push_back(Position(x, y));
    
    while (!stack.empty()) {
        Position pos = stack.back();
        stack.pop_back();
        
        if (visited.find(pos) != visited.end()) {
            continue;
        }
        visited.insert(pos);
        group.push_back(pos);
        
        auto neighbors = get_neighbors(pos.x, pos.y);
        for (const auto& n : neighbors) {
            if (visited.find(n) == visited.end() && get(n.x, n.y) == color) {
                stack.push_back(n);
            }
        }
    }
    
    return group;
}

bool Board::has_liberties(int x, int y) const {
    int color = get(x, y);
    if (color == EMPTY || color == -1) {
        return false;
    }
    
    std::unordered_set<Position, PositionHash> visited;
    std::vector<Position> stack;
    stack.push_back(Position(x, y));
    
    while (!stack.empty()) {
        Position pos = stack.back();
        stack.pop_back();
        
        if (visited.find(pos) != visited.end()) {
            continue;
        }
        visited.insert(pos);
        
        auto neighbors = get_neighbors(pos.x, pos.y);
        for (const auto& n : neighbors) {
            int neighbor_color = get(n.x, n.y);
            if (neighbor_color == EMPTY) {
                return true;
            }
            if (neighbor_color == color && visited.find(n) == visited.end()) {
                stack.push_back(n);
            }
        }
    }
    
    return false;
}

bool Board::group_has_liberties(const std::vector<Position>& group) const {
    for (const auto& pos : group) {
        if (has_liberties(pos.x, pos.y)) {
            return true;
        }
    }
    return false;
}

std::vector<std::vector<Position>> Board::capture_groups(int x, int y, int color) {
    std::vector<std::vector<Position>> captured;
    std::unordered_set<Position, PositionHash> visited;
    
    auto neighbors = get_neighbors(x, y);
    for (const auto& pos : neighbors) {
        if (visited.find(pos) != visited.end()) {
            continue;
        }
        if (get(pos.x, pos.y) == color) {
            auto group = get_group(pos.x, pos.y);
            if (!group_has_liberties(group)) {
                captured.push_back(group);
                for (const auto& p : group) {
                    visited.insert(p);
                    set(p.x, p.y, EMPTY);
                }
            }
        }
    }
    
    return captured;
}

bool Board::is_valid_move(int x, int y, int color) const {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false;
    }
    if (grid[y][x] != EMPTY) {
        return false;
    }
    
    // Check ko rule
    if (ko.has_value() && ko->x == x && ko->y == y) {
        return false;
    }
    
    // Try the move
    Board test_board = *this;
    test_board.set(x, y, color);
    
    // Capture opponent groups first
    auto captured = test_board.capture_groups(x, y, get_opponent(color));
    
    // If no opponent groups were captured, check if the move is suicide
    if (captured.empty()) {
        // Check if the newly placed stone's group has liberties
        if (!test_board.has_liberties(x, y)) {
            return false; // Suicide move - invalid
        }
    }
    
    return true;
}

bool Board::make_move(int x, int y, int color) {
    if (!is_valid_move(x, y, color)) {
        return false;
    }
    
    set(x, y, color);
    last_move = Position(x, y);
    
    // Capture opponent stones
    auto captured = capture_groups(x, y, get_opponent(color));
    if (captured.size() == 1 && captured[0].size() == 1) {
        ko = captured[0][0];
    } else {
        ko = std::nullopt;
    }
    
    return true;
}

std::vector<Position> Board::get_valid_moves(int color) const {
    std::vector<Position> moves;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (is_valid_move(x, y, color)) {
                moves.push_back(Position(x, y));
            }
        }
    }
    return moves;
}

int Board::get_territory_owner(int x, int y) const {
    // Returns BLACK, WHITE, or EMPTY if neutral
    if (get(x, y) != EMPTY) {
        return EMPTY;
    }
    
    std::unordered_set<Position, PositionHash> visited;
    std::vector<Position> stack;
    stack.push_back(Position(x, y));
    bool has_black = false;
    bool has_white = false;
    
    while (!stack.empty()) {
        Position pos = stack.back();
        stack.pop_back();
        
        if (visited.find(pos) != visited.end()) {
            continue;
        }
        visited.insert(pos);
        
        auto neighbors = get_neighbors(pos.x, pos.y);
        for (const auto& n : neighbors) {
            if (n.x < 0 || n.x >= BOARD_SIZE || n.y < 0 || n.y >= BOARD_SIZE) {
                continue;
            }
            int cell = get(n.x, n.y);
            if (cell == BLACK) {
                has_black = true;
            } else if (cell == WHITE) {
                has_white = true;
            } else if (cell == EMPTY) {
                if (visited.find(n) == visited.end()) {
                    stack.push_back(n);
                }
            }
        }
    }
    
    // If territory is surrounded by only one color, that player controls it
    if (has_black && !has_white) {
        return BLACK;
    } else if (has_white && !has_black) {
        return WHITE;
    } else {
        return EMPTY; // Neutral territory
    }
}

int get_opponent(int color) {
    if (color == BLACK) {
        return WHITE;
    } else {
        return BLACK;
    }
}

