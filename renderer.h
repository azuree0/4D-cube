// 4D Tesseract OpenGL Renderer Header
// Handles 4D projection and 3D rendering of the tesseract puzzle

#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "tesseract_model.h"
#include "rubik_cube.h"
#include "math_4d.h"
#include <vector>

// Animation state for inner 3x3x3 Rubik cube
struct RubikAnimState {
    int face;
    float currentAngle;
    float targetAngle;
    bool isAnimating;
    bool clockwise;
    RubikAnimState() : face(-1), currentAngle(0.0f), targetAngle(0.0f), isAnimating(false), clockwise(true) {}
};

// Animation state for 4D slice rotations
struct AnimationState {
    int plane;           // Rotation plane (PLANE_XY, PLANE_XZ, etc.)
    int layer;           // Layer index 0..3
    float currentAngle;  // Current rotation angle in degrees
    float targetAngle;  // Target (90 or -90)
    bool isAnimating;
    bool clockwise;

    AnimationState() : plane(-1), layer(-1), currentAngle(0.0f), targetAngle(0.0f), isAnimating(false), clockwise(true) {}
};

// Renderer - 4D projection and OpenGL drawing
class Renderer {
private:
    float cameraAngleX;
    float cameraAngleY;
    float cameraDistance;
    float viewAngleW_;   // 4D rotation angle (ZW plane) for viewing
    float wDistance_;    // 4D projection distance
    Vec4 outerPositions_[16];  // Outer vertex positions (updated by inner cube moves)

    void setColor(int cellColor);
    void setColorTranslucent(int cellColor, float alpha);
    void drawStars();
    void setColorRubik(int faceColor);
    void drawFace(float x, float y, float z, float size, int faceIndex, int color);
    void drawFaceTranslucent(float x, float y, float z, float size, int faceIndex, int color, float alpha);
    void drawFaceRubik(float x, float y, float z, float size, int faceIndex, int faceColor);
    void drawCube(float x, float y, float z, float size);
    void drawCubeTranslucent(float x, float y, float z, float size, float alpha);
    void drawCubieRubik(float x, float y, float z, float size, const RubikCube& cube, int cx, int cy, int cz, const RubikAnimState& anim);
    void drawEdge(const Vec4& a, const Vec4& b, const Mat4x4& viewRot, float wDist);
    void drawVertex(const Vec4& pos, const Vertex4D& v, const Mat4x4& viewRot, float wDist);
    Mat4x4 getViewRotation4D() const;
    Mat4x4 getAnimationRotation(const AnimationState& anim) const;

public:
    Renderer();

    void initialize();
    void render(const TesseractPuzzle& puzzle, const RubikCube* innerCube, int windowWidth, int windowHeight,
                const AnimationState& anim, const RubikAnimState& rubikAnim);
    void handleMouseDrag(int deltaX, int deltaY);
    void handleMouseWheel(int delta);
    void rotate4DView(float deltaAngle);
    void resetCamera();
    void commitOuterRubikRotation(int face, bool clockwise);  // Call when inner cube move completes
    void resetOuterPositions();
};

#endif // RENDERER_H
