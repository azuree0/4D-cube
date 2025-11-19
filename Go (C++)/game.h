#ifndef GAME_H
#define GAME_H

#include "board.h"
#include <vector>

struct GameState {
    Board board;
    int current_player;
    bool black_pass;
    bool white_pass;
    bool game_over;
    
    GameState(const Board& b, int cp, bool bp, bool wp, bool go)
        : board(b), current_player(cp), black_pass(bp), white_pass(wp), game_over(go) {}
};

class Game {
private:
    Board board;
    int current_player;
    bool black_pass;
    bool white_pass;
    bool game_over;
    std::vector<GameState> history;
    
    void save_state();

public:
    Game();
    
    void reset();
    bool undo();
    bool make_move(int x, int y);
    int get_current_player() const;
    const Board& get_board() const;
    bool is_game_over() const;
    std::vector<std::vector<int>> get_board_state() const;
    std::pair<int, int> calculate_score() const; // Returns (black_score, white_score)
};

#endif // GAME_H

