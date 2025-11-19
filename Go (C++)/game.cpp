#include "game.h"

Game::Game() {
    board = Board();
    current_player = BLACK;
    black_pass = false;
    white_pass = false;
    game_over = false;
}

void Game::reset() {
    board = Board();
    current_player = BLACK;
    black_pass = false;
    white_pass = false;
    game_over = false;
    history.clear();
}

void Game::save_state() {
    history.push_back(GameState(board, current_player, black_pass, white_pass, game_over));
}

bool Game::undo() {
    if (history.empty()) {
        return false;
    }
    GameState state = history.back();
    history.pop_back();
    board = state.board;
    current_player = state.current_player;
    black_pass = state.black_pass;
    white_pass = state.white_pass;
    game_over = state.game_over;
    return true;
}

bool Game::make_move(int x, int y) {
    if (game_over) {
        return false;
    }
    
    // Save state before making move
    save_state();
    
    if (x < 0 || y < 0) {
        // Pass move
        if (current_player == BLACK) {
            black_pass = true;
        } else {
            white_pass = true;
        }
        
        if (black_pass && white_pass) {
            game_over = true;
        }
        current_player = get_opponent(current_player);
        return true;
    }
    
    if (board.make_move(x, y, current_player)) {
        if (current_player == BLACK) {
            black_pass = false;
        } else {
            white_pass = false;
        }
        current_player = get_opponent(current_player);
        return true;
    }
    
    // If move failed, remove the saved state
    history.pop_back();
    return false;
}

int Game::get_current_player() const {
    return current_player;
}

const Board& Game::get_board() const {
    return board;
}

bool Game::is_game_over() const {
    return game_over;
}

std::vector<std::vector<int>> Game::get_board_state() const {
    std::vector<std::vector<int>> state;
    for (int y = 0; y < BOARD_SIZE; y++) {
        std::vector<int> row;
        for (int x = 0; x < BOARD_SIZE; x++) {
            row.push_back(board.get(x, y));
        }
        state.push_back(row);
    }
    return state;
}

std::pair<int, int> Game::calculate_score() const {
    // Returns (black_score, white_score)
    // Using area scoring: stones on board + controlled territory
    int black_score = 0;
    int white_score = 0;
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            int cell = board.get(x, y);
            if (cell == BLACK) {
                black_score += 1;
            } else if (cell == WHITE) {
                white_score += 1;
            } else if (cell == EMPTY) {
                // Check which player controls this empty space
                int controlled_by = board.get_territory_owner(x, y);
                if (controlled_by == BLACK) {
                    black_score += 1;
                } else if (controlled_by == WHITE) {
                    white_score += 1;
                }
                // Neutral territory doesn't count
            }
        }
    }
    
    return std::make_pair(black_score, white_score);
}

