// Rubik's Cube Logic Header
// Defines the cube data structure and rotation operations

#ifndef RUBIK_CUBE_H
#define RUBIK_CUBE_H

#include <vector>
#include <string>

// Face colors: 0=White, 1=Yellow, 2=Red, 3=Orange, 4=Green, 5=Blue
enum FaceColor {
    WHITE = 0,
    YELLOW = 1,
    RED = 2,
    ORANGE = 3,
    GREEN = 4,
    BLUE = 5
};

// Face indices: 0=Right, 1=Left, 2=Up, 3=Down, 4=Front, 5=Back
enum FaceIndex {
    RIGHT = 0,
    LEFT = 1,
    UP = 2,
    DOWN = 3,
    FRONT = 4,
    BACK = 5
};

// Rubik's Cube class - manages cube state and rotations
class RubikCube {
private:
    // 6 faces, each is 3x3 grid of colors
    std::vector<std::vector<std::vector<int>>> faces;
    
    // Helper functions for face and edge rotations
    void rotateFaceClockwise(int face);
    void rotateFaceCounterClockwise(int face);
    void rotateRow(int row, bool clockwise);
    void rotateColumn(int col, bool clockwise);
    void rotateDepth(int depth, bool clockwise);
    
public:
    RubikCube();  // Constructor - initializes solved cube
    
    // Reset cube to solved state
    void reset();
    
    // Face rotations (clockwise): R, L, U, D, F, B
    void rotateR();  // Right face clockwise
    void rotateL();  // Left face clockwise
    void rotateU();  // Up face clockwise
    void rotateD();  // Down face clockwise
    void rotateF();  // Front face clockwise
    void rotateB();  // Back face clockwise
    
    void rotateRPrime();  // Right face counter-clockwise
    void rotateLPrime();  // Left face counter-clockwise
    void rotateUPrime();  // Up face counter-clockwise
    void rotateDPrime();  // Down face counter-clockwise
    void rotateFPrime();  // Front face counter-clockwise
    void rotateBPrime();  // Back face counter-clockwise
    
    // Apply move from string notation (e.g., "R", "R'", "U", "U'")
    bool applyMove(const std::string& move);
    
    // Scramble the cube
    void scramble(int numMoves = 25);
    
    // Check if cube is solved
    bool isSolved() const;
    
    // Get face color at position (face, row, col)
    int getColor(int face, int row, int col) const;
    
    // Get all faces (for rendering)
    const std::vector<std::vector<std::vector<int>>>& getFaces() const;
};

#endif // RUBIK_CUBE_H

