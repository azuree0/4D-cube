#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include "rubik_cube.h"
#include "renderer.h"

constexpr int WINDOW_WIDTH = 1400;
constexpr int WINDOW_HEIGHT = 1000;

class RubikGame {
private:
    RubikCube cube;
    Renderer renderer;
    sf::Font font;
    sf::Text statusText;
    sf::Text instructionText;
    bool isDragging;
    sf::Vector2i lastMousePos;
    bool showInstructions;
    AnimationState animation;
    sf::Clock animationClock;
    const float ANIMATION_SPEED = 300.0f; // degrees per second
    
    bool loadFont() {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
                std::cerr << "Warning: Could not load font. Text may not display correctly." << std::endl;
                return false;
            }
        }
        return true;
    }
    
    void setupUI() {
        if (font.getInfo().family != "") {
            statusText.setFont(font);
            statusText.setCharacterSize(24);
            statusText.setFillColor(sf::Color::White);
            statusText.setPosition(10, 10);
            
            instructionText.setFont(font);
            instructionText.setCharacterSize(18);
            instructionText.setFillColor(sf::Color::White);
            instructionText.setPosition(10, 40);
            instructionText.setString(
                "Controls:\n"
                "Mouse Drag: Rotate camera\n"
                "Mouse Wheel: Zoom in/out\n"
                "Q/W/E/R/T/Y: Rotate faces (Right/Left/Up/Down/Front/Back)\n"
                "Shift+Q/W/E/R/T/Y: Rotate counter-clockwise\n"
                "S: Scramble\n"
                "Space: Reset cube\n"
                "I: Toggle instructions"
            );
        }
        updateUI();
    }
    
    void updateUI() {
        if (font.getInfo().family == "") return;
        
        std::string status = "Rubik's Cube";
        if (cube.isSolved()) {
            status += " - Solved";
        }
        statusText.setString(status);
    }
    
public:
    RubikGame() : isDragging(false), showInstructions(true) {
        loadFont();
        setupUI();
        renderer.initialize();
    }
    
    void updateAnimation(float deltaTime) {
        if (!animation.isAnimating) return;
        
        float angleDelta = ANIMATION_SPEED * deltaTime;
        if (animation.clockwise) {
            animation.currentAngle += angleDelta;
            if (animation.currentAngle >= animation.targetAngle) {
                animation.currentAngle = animation.targetAngle;
                animation.isAnimating = false;
                // Apply the actual rotation to the cube
                applyRotationToCube();
            }
        } else {
            animation.currentAngle -= angleDelta;
            if (animation.currentAngle <= animation.targetAngle) {
                animation.currentAngle = animation.targetAngle;
                animation.isAnimating = false;
                // Apply the actual rotation to the cube
                applyRotationToCube();
            }
        }
    }
    
    void startAnimation(int face, bool clockwise) {
        if (animation.isAnimating) return; // Don't start new animation if one is in progress
        
        animation.face = face;
        animation.clockwise = clockwise;
        animation.currentAngle = 0.0f;
        animation.targetAngle = clockwise ? 90.0f : -90.0f;
        animation.isAnimating = true;
        animationClock.restart();
    }
    
    void applyRotationToCube() {
        switch (animation.face) {
            case RIGHT:
                if (animation.clockwise) cube.rotateR();
                else cube.rotateRPrime();
                break;
            case LEFT:
                if (animation.clockwise) cube.rotateL();
                else cube.rotateLPrime();
                break;
            case UP:
                if (animation.clockwise) cube.rotateU();
                else cube.rotateUPrime();
                break;
            case DOWN:
                if (animation.clockwise) cube.rotateD();
                else cube.rotateDPrime();
                break;
            case FRONT:
                if (animation.clockwise) cube.rotateF();
                else cube.rotateFPrime();
                break;
            case BACK:
                if (animation.clockwise) cube.rotateB();
                else cube.rotateBPrime();
                break;
        }
        updateUI();
    }
    
    void handleKeyPress(sf::Keyboard::Key key) {
        if (animation.isAnimating) return; // Ignore input during animation
        
        bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || 
                     sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
        
        switch (key) {
            case sf::Keyboard::Q:
                startAnimation(RIGHT, !shift);
                break;
            case sf::Keyboard::W:
                startAnimation(LEFT, !shift);
                break;
            case sf::Keyboard::E:
                startAnimation(UP, !shift);
                break;
            case sf::Keyboard::R:
                startAnimation(DOWN, !shift);
                break;
            case sf::Keyboard::T:
                startAnimation(FRONT, !shift);
                break;
            case sf::Keyboard::Y:
                startAnimation(BACK, !shift);
                break;
            case sf::Keyboard::S:
                cube.scramble();
                updateUI();
                break;
            case sf::Keyboard::Space:
                cube.reset();
                animation.isAnimating = false;
                updateUI();
                break;
            case sf::Keyboard::I:
                showInstructions = !showInstructions;
                break;
            default:
                break;
        }
    }
    
    void handleMouseButtonPressed(sf::Vector2i mousePos) {
        isDragging = true;
        lastMousePos = mousePos;
    }
    
    void handleMouseButtonReleased() {
        isDragging = false;
    }
    
    void handleMouseMove(sf::Vector2i mousePos) {
        if (isDragging) {
            int deltaX = mousePos.x - lastMousePos.x;
            int deltaY = mousePos.y - lastMousePos.y;
            renderer.handleMouseDrag(deltaX, deltaY);
            lastMousePos = mousePos;
        }
    }
    
    void handleMouseWheel(int delta) {
        renderer.handleMouseWheel(delta);
    }
    
    void render(sf::RenderWindow& window) {
        // Render 3D cube using OpenGL
        renderer.render(cube, window.getSize().x, window.getSize().y, animation);
        
        // Switch to SFML 2D rendering for UI
        window.pushGLStates();
        
        if (font.getInfo().family != "") {
            window.draw(statusText);
            if (showInstructions) {
                window.draw(instructionText);
            }
        }
        
        window.popGLStates();
        window.display();
    }
};

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 2;
    settings.minorVersion = 1;
    
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
                           "3D Rubik's Cube", 
                           sf::Style::Default, 
                           settings);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    
    // Enable OpenGL context
    window.setActive(true);
    
    RubikGame game;
    sf::Clock frameClock;
    
    while (window.isOpen()) {
        float deltaTime = frameClock.restart().asSeconds();
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                game.handleKeyPress(event.key.code);
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    game.handleMouseButtonPressed(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    game.handleMouseButtonReleased();
                }
            } else if (event.type == sf::Event::MouseMoved) {
                game.handleMouseMove(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            } else if (event.type == sf::Event::MouseWheelScrolled) {
                game.handleMouseWheel(static_cast<int>(event.mouseWheelScroll.delta));
            } else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }
        
        // Update animation
        game.updateAnimation(deltaTime);
        
        game.render(window);
    }
    
    return 0;
}

