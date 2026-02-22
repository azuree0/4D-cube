// Main Application - SFML window and game loop
// 4D Tesseract puzzle with plane/layer controls (SFML 3 compatible)

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <optional>
#include <exception>
#include "rubik_cube.h"
#include "renderer.h"

constexpr int WINDOW_WIDTH = 1400;
constexpr int WINDOW_HEIGHT = 1000;

class TesseractGame {
private:
    TesseractPuzzle puzzle;
    RubikCube innerCube;
    Renderer renderer;
    sf::Font font;
    std::optional<sf::Text> statusText;
    std::optional<sf::Text> instructionText;
    bool isDragging;
    sf::Vector2i lastMousePos;
    bool showInstructions;
    AnimationState animation;
    RubikAnimState rubikAnim;
    sf::Clock animationClock;
    const float ANIMATION_SPEED = 300.0f;
    int currentLayer_;

    bool loadFont() {
        if (font.openFromFile("C:/Windows/Fonts/arial.ttf"))
            return true;
        if (font.openFromFile("C:/Windows/Fonts/calibri.ttf"))
            return true;
        std::cerr << "Warning: Could not load font. Text may not display correctly." << std::endl;
        return false;
    }

    void setupUI() {
        if (!font.getInfo().family.empty()) {
            statusText.emplace(font, "", 24);
            statusText->setFillColor(sf::Color::White);
            statusText->setPosition({10.f, 10.f});

            instructionText.emplace(font,
                "Mouse Drag: Rotate camera | Wheel: Zoom\n"
                "\n"
                "4D cube: Q/W/E/R/T/Y\n"
                "Shift + key: Counter-clockwise\n"
                "\n"
                "Space: Reset | I: Toggle UI",
                18);
            instructionText->setFillColor(sf::Color::White);
            instructionText->setPosition({10.f, 50.f});
        }
        updateUI();
    }

    void updateUI() {
        if (!statusText) return;
        std::string status = puzzle.isSolved() ? "Solved " : "";
        statusText->setString(status);
    }

public:
    TesseractGame() : isDragging(false), showInstructions(true), currentLayer_(0) {
        loadFont();
        setupUI();
        renderer.initialize();
        puzzle.scramble();
        updateUI();
    }

    void updateAnimation(float deltaTime) {
        float angleDelta = ANIMATION_SPEED * deltaTime;
        if (rubikAnim.isAnimating) {
            if (rubikAnim.clockwise) {
                rubikAnim.currentAngle += angleDelta;
                if (rubikAnim.currentAngle >= rubikAnim.targetAngle) {
                    rubikAnim.currentAngle = rubikAnim.targetAngle;
                    rubikAnim.isAnimating = false;
                    applyRubikRotation();
                }
            } else {
                rubikAnim.currentAngle -= angleDelta;
                if (rubikAnim.currentAngle <= rubikAnim.targetAngle) {
                    rubikAnim.currentAngle = rubikAnim.targetAngle;
                    rubikAnim.isAnimating = false;
                    applyRubikRotation();
                }
            }
            return;
        }
        if (!animation.isAnimating) return;
        if (animation.clockwise) {
            animation.currentAngle += angleDelta;
            if (animation.currentAngle >= animation.targetAngle) {
                animation.currentAngle = animation.targetAngle;
                animation.isAnimating = false;
                applyRotationToPuzzle();
            }
        } else {
            animation.currentAngle -= angleDelta;
            if (animation.currentAngle <= animation.targetAngle) {
                animation.currentAngle = animation.targetAngle;
                animation.isAnimating = false;
                applyRotationToPuzzle();
            }
        }
    }

    void startRubikAnimation(int face, bool clockwise) {
        if (rubikAnim.isAnimating || animation.isAnimating) return;
        rubikAnim.face = face;
        rubikAnim.clockwise = clockwise;
        rubikAnim.currentAngle = 0.0f;
        rubikAnim.targetAngle = clockwise ? 90.0f : -90.0f;
        rubikAnim.isAnimating = true;
    }

    void applyRubikRotation() {
        if (rubikAnim.face >= 0) {
            switch (rubikAnim.face) {
                case RIGHT: rubikAnim.clockwise ? innerCube.rotateR() : innerCube.rotateRPrime(); break;
                case LEFT:  rubikAnim.clockwise ? innerCube.rotateL() : innerCube.rotateLPrime(); break;
                case UP:    rubikAnim.clockwise ? innerCube.rotateU() : innerCube.rotateUPrime(); break;
                case DOWN:  rubikAnim.clockwise ? innerCube.rotateD() : innerCube.rotateDPrime(); break;
                case FRONT: rubikAnim.clockwise ? innerCube.rotateF() : innerCube.rotateFPrime(); break;
                case BACK:  rubikAnim.clockwise ? innerCube.rotateB() : innerCube.rotateBPrime(); break;
            }
            renderer.commitOuterRubikRotation(rubikAnim.face, rubikAnim.clockwise);
        }
        updateUI();
    }

    void startAnimation(int plane, int layer, bool clockwise) {
        if (animation.isAnimating || rubikAnim.isAnimating) return;
        animation.plane = plane;
        animation.layer = layer;
        animation.clockwise = clockwise;
        animation.currentAngle = 0.0f;
        animation.targetAngle = clockwise ? 90.0f : -90.0f;
        animation.isAnimating = true;
    }

    void applyRotationToPuzzle() {
        if (animation.plane >= 0 && animation.layer >= 0) {
            puzzle.rotateSlice(animation.plane, animation.layer, animation.clockwise);
        }
        updateUI();
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Key::LBracket) { renderer.rotate4DView(-5.0f); return; }
        if (key == sf::Keyboard::Key::RBracket) { renderer.rotate4DView(5.0f); return; }
        if (animation.isAnimating || rubikAnim.isAnimating) return;
        bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
                     sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

        switch (key) {
            case sf::Keyboard::Key::Q: startRubikAnimation(RIGHT, !shift); break;
            case sf::Keyboard::Key::W: startRubikAnimation(LEFT, !shift); break;
            case sf::Keyboard::Key::E: startRubikAnimation(UP, !shift); break;
            case sf::Keyboard::Key::R: startRubikAnimation(DOWN, !shift); break;
            case sf::Keyboard::Key::T: startRubikAnimation(FRONT, !shift); break;
            case sf::Keyboard::Key::Y: startRubikAnimation(BACK, !shift); break;
            case sf::Keyboard::Key::Z: startAnimation(PLANE_XY, currentLayer_, !shift); break;
            case sf::Keyboard::Key::X: startAnimation(PLANE_XZ, currentLayer_, !shift); break;
            case sf::Keyboard::Key::C: startAnimation(PLANE_XW, currentLayer_, !shift); break;
            case sf::Keyboard::Key::V: startAnimation(PLANE_YZ, currentLayer_, !shift); break;
            case sf::Keyboard::Key::B: startAnimation(PLANE_YW, currentLayer_, !shift); break;
            case sf::Keyboard::Key::N: startAnimation(PLANE_ZW, currentLayer_, !shift); break;
            case sf::Keyboard::Key::Num1: currentLayer_ = 0; updateUI(); break;
            case sf::Keyboard::Key::Num2: currentLayer_ = 1; updateUI(); break;
            case sf::Keyboard::Key::Num3: currentLayer_ = 2; updateUI(); break;
            case sf::Keyboard::Key::Num4: currentLayer_ = 3; updateUI(); break;
            case sf::Keyboard::Key::Space:
                puzzle.reset();
                innerCube.reset();
                renderer.resetOuterPositions();
                animation.isAnimating = false;
                rubikAnim.isAnimating = false;
                updateUI();
                break;
            case sf::Keyboard::Key::I:
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
        if (!window.setActive(true)) return;  // Ensure OpenGL context is active before GL calls
        renderer.render(puzzle, &innerCube, static_cast<int>(window.getSize().x), static_cast<int>(window.getSize().y), animation, rubikAnim);
        window.pushGLStates();
        if (statusText) {
            window.draw(*statusText);
            if (showInstructions && instructionText) window.draw(*instructionText);
        }
        window.popGLStates();
        window.display();
    }
};

int main() {
    try {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antiAliasingLevel = 0;  // 4 can fail on some GPUs; 0 is more compatible
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}),
                           "Tesseract",
                           sf::Style::Default,
                           sf::State::Windowed,
                           settings);
    if (!window.isOpen()) {
        std::cerr << "Failed to create window." << std::endl;
        return 1;
    }
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    if (!window.setActive(true)) {
        std::cerr << "Failed to activate OpenGL context." << std::endl;
        return 1;
    }

    TesseractGame game;
    sf::Clock frameClock;

    while (window.isOpen()) {
        float deltaTime = frameClock.restart().asSeconds();

        while (std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (event->is<sf::Event::KeyPressed>()) {
                if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
                    game.handleKeyPress(k->code);
                }
            } else if (event->is<sf::Event::MouseButtonPressed>()) {
                if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>()) {
                    if (m->button == sf::Mouse::Button::Left) {
                        game.handleMouseButtonPressed(m->position);
                    }
                }
            } else if (event->is<sf::Event::MouseButtonReleased>()) {
                if (const auto* m = event->getIf<sf::Event::MouseButtonReleased>()) {
                    if (m->button == sf::Mouse::Button::Left) {
                        game.handleMouseButtonReleased();
                    }
                }
            } else if (event->is<sf::Event::MouseMoved>()) {
                if (const auto* m = event->getIf<sf::Event::MouseMoved>()) {
                    game.handleMouseMove(m->position);
                }
            } else if (event->is<sf::Event::MouseWheelScrolled>()) {
                if (const auto* m = event->getIf<sf::Event::MouseWheelScrolled>()) {
                    game.handleMouseWheel(static_cast<int>(m->delta));
                }
            } else if (event->is<sf::Event::Resized>()) {
                if (const auto* r = event->getIf<sf::Event::Resized>()) {
                    glViewport(0, 0, static_cast<GLsizei>(r->size.x), static_cast<GLsizei>(r->size.y));
                }
            }
        }

        game.updateAnimation(deltaTime);
        game.render(window);
    }

    return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
        return 1;
    }
}
