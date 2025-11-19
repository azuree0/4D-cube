#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <sstream>
#include "game.h"

constexpr int WINDOW_WIDTH = 1400;
constexpr int WINDOW_HEIGHT = 1600;
constexpr int BOARD_PADDING = 50;
constexpr int CELL_SIZE = 50;
constexpr int STONE_RADIUS = 22;
constexpr int BOARD_SIZE_PIXELS = BOARD_PADDING * 2 + (BOARD_SIZE - 1) * CELL_SIZE;
constexpr int BOARD_OFFSET_X = (WINDOW_WIDTH - BOARD_SIZE_PIXELS) / 2;
constexpr int BOARD_OFFSET_Y = 180;

sf::Color BOARD_COLOR(222, 184, 135); // #deb887
sf::Color LINE_COLOR(0, 0, 0);
sf::Color BLACK_STONE(0, 0, 0);
sf::Color WHITE_STONE(255, 255, 255);
sf::Color BACKGROUND(255, 255, 255);

class GoGame {
private:
    Game game;
    sf::Font font;
    sf::Text currentPlayerText;
    sf::Text statusText;
    sf::Text scoreText;
    std::string statusMessage;
    sf::Color statusColor;
    
    bool loadFont() {
        // Try to load a system font or use default
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            // Try alternative fonts
            if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
                std::cerr << "Warning: Could not load font. Text may not display correctly." << std::endl;
                return false;
            }
        }
        return true;
    }
    
    void setupUI() {
        if (font.getInfo().family != "") {
            currentPlayerText.setFont(font);
            currentPlayerText.setCharacterSize(36);
            currentPlayerText.setFillColor(sf::Color::Black);
            currentPlayerText.setPosition(30, 30);
            
            statusText.setFont(font);
            statusText.setCharacterSize(28);
            statusText.setPosition(30, 80);
            
            scoreText.setFont(font);
            scoreText.setCharacterSize(28);
            scoreText.setFillColor(sf::Color::Black);
            scoreText.setPosition(30, 1500);
        }
        updateUI();
    }
    
    void updateUI() {
        std::string playerName = (game.get_current_player() == BLACK) ? "Black" : "White";
        if (font.getInfo().family != "") {
            currentPlayerText.setString("Current Player: " + playerName);
        }
        
        if (game.is_game_over()) {
            auto scores = game.calculate_score();
            int blackScore = scores.first;
            int whiteScore = scores.second;
            std::string winner;
            if (blackScore > whiteScore) {
                winner = "Black wins!";
            } else if (whiteScore > blackScore) {
                winner = "White wins!";
            } else {
                winner = "Tie!";
            }
            statusMessage = winner + " | Black: " + std::to_string(blackScore) + 
                          " - White: " + std::to_string(whiteScore);
            statusColor = sf::Color::Black;
            
            if (font.getInfo().family != "") {
                scoreText.setString(statusMessage);
            }
        } else {
            statusMessage = "";
        }
        
        if (font.getInfo().family != "" && !statusMessage.empty()) {
            statusText.setString(statusMessage);
            statusText.setFillColor(statusColor);
        }
    }
    
    sf::Vector2i getBoardCoords(int mouseX, int mouseY) const {
        int x = mouseX - BOARD_OFFSET_X;
        int y = mouseY - BOARD_OFFSET_Y;
        
        int boardX = std::round((float)(x - BOARD_PADDING) / CELL_SIZE);
        int boardY = std::round((float)(y - BOARD_PADDING) / CELL_SIZE);
        
        if (boardX >= 0 && boardX < BOARD_SIZE && boardY >= 0 && boardY < BOARD_SIZE) {
            return sf::Vector2i(boardX, boardY);
        }
        return sf::Vector2i(-1, -1);
    }
    
    void drawBoard(sf::RenderWindow& window) {
        // Draw board background
        sf::RectangleShape boardBg(sf::Vector2f(BOARD_SIZE_PIXELS, BOARD_SIZE_PIXELS));
        boardBg.setPosition(BOARD_OFFSET_X, BOARD_OFFSET_Y);
        boardBg.setFillColor(BOARD_COLOR);
        boardBg.setOutlineColor(sf::Color(139, 105, 20));
        boardBg.setOutlineThickness(2);
        window.draw(boardBg);
        
        // Draw grid lines
        sf::RectangleShape line(sf::Vector2f(1, (BOARD_SIZE - 1) * CELL_SIZE));
        line.setFillColor(LINE_COLOR);
        
        for (int i = 0; i < BOARD_SIZE; i++) {
            int pos = BOARD_PADDING + i * CELL_SIZE;
            
            // Vertical lines
            line.setSize(sf::Vector2f(1, (BOARD_SIZE - 1) * CELL_SIZE));
            line.setPosition(BOARD_OFFSET_X + pos, BOARD_OFFSET_Y + BOARD_PADDING);
            window.draw(line);
            
            // Horizontal lines
            line.setSize(sf::Vector2f((BOARD_SIZE - 1) * CELL_SIZE, 1));
            line.setPosition(BOARD_OFFSET_X + BOARD_PADDING, BOARD_OFFSET_Y + pos);
            window.draw(line);
        }
        
        // Draw star points (hoshi)
        const int starPoints[] = {3, 9, 15};
        sf::CircleShape star(5);
        star.setFillColor(LINE_COLOR);
        for (int x : starPoints) {
            for (int y : starPoints) {
                star.setPosition(
                    BOARD_OFFSET_X + BOARD_PADDING + x * CELL_SIZE - 5,
                    BOARD_OFFSET_Y + BOARD_PADDING + y * CELL_SIZE - 5
                );
                window.draw(star);
            }
        }
        
        // Draw stones
        auto boardState = game.get_board_state();
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                int color = boardState[y][x];
                if (color != EMPTY) {
                    drawStone(window, x, y, color);
                }
            }
        }
    }
    
    void drawStone(sf::RenderWindow& window, int x, int y, int color) {
        float centerX = BOARD_OFFSET_X + BOARD_PADDING + x * CELL_SIZE;
        float centerY = BOARD_OFFSET_Y + BOARD_PADDING + y * CELL_SIZE;
        
        sf::CircleShape stone(STONE_RADIUS);
        stone.setPosition(centerX - STONE_RADIUS, centerY - STONE_RADIUS);
        
        if (color == BLACK) {
            stone.setFillColor(BLACK_STONE);
            stone.setOutlineColor(sf::Color(51, 51, 51));
        } else {
            stone.setFillColor(WHITE_STONE);
            stone.setOutlineColor(sf::Color(51, 51, 51));
        }
        stone.setOutlineThickness(1);
        window.draw(stone);
    }
    
    void drawButtons(sf::RenderWindow& window) {
        // Draw button backgrounds and text
        if (font.getInfo().family == "") return;
        
        // New Game button
        sf::RectangleShape newGameBtn(sf::Vector2f(180, 50));
        newGameBtn.setPosition(30, 120);
        newGameBtn.setFillColor(BACKGROUND);
        newGameBtn.setOutlineColor(sf::Color::Black);
        newGameBtn.setOutlineThickness(3);
        window.draw(newGameBtn);
        
        sf::Text newGameText("New Game", font, 24);
        newGameText.setFillColor(sf::Color::Black);
        newGameText.setPosition(50, 130);
        window.draw(newGameText);
        
        // Undo button
        sf::RectangleShape undoBtn(sf::Vector2f(180, 50));
        undoBtn.setPosition(230, 120);
        undoBtn.setFillColor(BACKGROUND);
        undoBtn.setOutlineColor(sf::Color::Black);
        undoBtn.setOutlineThickness(3);
        if (!game.is_game_over()) {
            window.draw(undoBtn);
            
            sf::Text undoText("Undo", font, 24);
            undoText.setFillColor(sf::Color::Black);
            undoText.setPosition(280, 130);
            window.draw(undoText);
        } else {
            undoBtn.setFillColor(sf::Color(200, 200, 200));
            window.draw(undoBtn);
            
            sf::Text undoText("Undo", font, 24);
            undoText.setFillColor(sf::Color(128, 128, 128));
            undoText.setPosition(280, 130);
            window.draw(undoText);
        }
        
        // Pass button
        sf::RectangleShape passBtn(sf::Vector2f(180, 50));
        passBtn.setPosition(430, 120);
        passBtn.setFillColor(BACKGROUND);
        passBtn.setOutlineColor(sf::Color::Black);
        passBtn.setOutlineThickness(3);
        if (!game.is_game_over()) {
            window.draw(passBtn);
            
            sf::Text passText("Pass", font, 24);
            passText.setFillColor(sf::Color::Black);
            passText.setPosition(480, 130);
            window.draw(passText);
        } else {
            passBtn.setFillColor(sf::Color(200, 200, 200));
            window.draw(passBtn);
            
            sf::Text passText("Pass", font, 24);
            passText.setFillColor(sf::Color(128, 128, 128));
            passText.setPosition(480, 130);
            window.draw(passText);
        }
    }
    
    bool isButtonClicked(int x, int y, int btnX, int btnY, int btnWidth, int btnHeight) {
        return x >= btnX && x <= btnX + btnWidth && y >= btnY && y <= btnY + btnHeight;
    }

public:
    GoGame() {
        loadFont();
        setupUI();
    }
    
    void handleClick(int mouseX, int mouseY) {
        // Check "New Game" button first - this should work even when game is over
        if (isButtonClicked(mouseX, mouseY, 30, 120, 180, 50)) {
            game.reset();
            updateUI();
            return;
        }
        
        // If game is over, don't process other clicks
        if (game.is_game_over()) return;
        
        // Check other button clicks (only when game is not over)
        if (isButtonClicked(mouseX, mouseY, 230, 120, 180, 50)) {
            if (game.undo()) {
                updateUI();
            }
            return;
        }
        
        if (isButtonClicked(mouseX, mouseY, 430, 120, 180, 50)) {
            game.make_move(-1, -1); // Pass
            updateUI();
            return;
        }
        
        // Check board click
        sf::Vector2i coords = getBoardCoords(mouseX, mouseY);
        if (coords.x >= 0 && coords.y >= 0) {
            if (game.make_move(coords.x, coords.y)) {
                updateUI();
            }
        }
    }
    
    void render(sf::RenderWindow& window) {
        window.clear(BACKGROUND);
        drawBoard(window);
        drawButtons(window);
        
        if (font.getInfo().family != "") {
            window.draw(currentPlayerText);
            if (!statusMessage.empty()) {
                window.draw(statusText);
            }
            if (game.is_game_over()) {
                window.draw(scoreText);
            }
        }
        
        window.display();
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Go 19x19 (C++)");
    window.setFramerateLimit(60);
    
    GoGame goGame;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    goGame.handleClick(event.mouseButton.x, event.mouseButton.y);
                }
            }
        }
        
        goGame.render(window);
    }
    
    return 0;
}

