#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "rubik_cube.h"
#include <vector>

// Animation state for face rotations
struct AnimationState {
    int face;           // Which face is rotating (RIGHT, LEFT, UP, DOWN, FRONT, BACK)
    float currentAngle;  // Current rotation angle in degrees
    float targetAngle;  // Target rotation angle (90 or -90)
    bool isAnimating;   // Whether animation is in progress
    bool clockwise;     // Rotation direction
    
    AnimationState() : face(-1), currentAngle(0.0f), targetAngle(0.0f), isAnimating(false), clockwise(true) {}
};

class Renderer {
private:
    float cameraAngleX;
    float cameraAngleY;
    float cameraDistance;
    
    // Color mapping
    void setColor(int faceColor);
    void drawCube(float x, float y, float z, float size);
    void drawFace(float x, float y, float z, float size, int faceIndex, int color);
    void drawCubie(float x, float y, float z, float size, const RubikCube& cube, int cubieX, int cubieY, int cubieZ, const AnimationState& anim);
    void drawStars();
    
public:
    Renderer();
    
    void initialize();
    void render(const RubikCube& cube, int windowWidth, int windowHeight, const AnimationState& anim);
    void handleMouseDrag(int deltaX, int deltaY);
    void handleMouseWheel(int delta);
    void resetCamera();
    
    // Get camera angles for UI
    float getCameraAngleX() const { return cameraAngleX; }
    float getCameraAngleY() const { return cameraAngleY; }
};

#endif // RENDERER_H

