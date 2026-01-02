// Rubik's Cube Implementation
// Contains all rotation logic and cube state management

#include "rubik_cube.h"
#include <algorithm>
#include <random>
#include <ctime>

// Constructor - initialize cube to solved state
RubikCube::RubikCube() {
    faces.resize(6);
    // Initialize each face with its correct color
    // RIGHT=0 -> RED, LEFT=1 -> ORANGE, UP=2 -> WHITE, DOWN=3 -> YELLOW, FRONT=4 -> GREEN, BACK=5 -> BLUE
    int faceColors[] = {RED, ORANGE, WHITE, YELLOW, GREEN, BLUE};
    for (int i = 0; i < 6; i++) {
        faces[i].resize(3);
        for (int j = 0; j < 3; j++) {
            faces[i][j].resize(3, faceColors[i]);
        }
    }
}

// Reset cube to solved state
void RubikCube::reset() {
    int faceColors[] = {RED, ORANGE, WHITE, YELLOW, GREEN, BLUE};
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                faces[i][j][k] = faceColors[i];
            }
        }
    }
}

// Rotate a single face 90 degrees clockwise
void RubikCube::rotateFaceClockwise(int face) {
    std::vector<std::vector<int>> temp(3, std::vector<int>(3));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[j][2 - i] = faces[face][i][j];
        }
    }
    faces[face] = temp;
}

// Rotate a single face 90 degrees counter-clockwise
void RubikCube::rotateFaceCounterClockwise(int face) {
    std::vector<std::vector<int>> temp(3, std::vector<int>(3));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            temp[2 - j][i] = faces[face][i][j];
        }
    }
    faces[face] = temp;
}

// Rotate right face clockwise (R move)
void RubikCube::rotateR() {
    rotateFaceClockwise(RIGHT);
    
    // Rotate adjacent edge pieces
    int temp[3];
    for (int i = 0; i < 3; i++) {
        temp[i] = faces[UP][i][2];
    }
    for (int i = 0; i < 3; i++) {
        faces[UP][i][2] = faces[FRONT][i][2];
    }
    for (int i = 0; i < 3; i++) {
        faces[FRONT][i][2] = faces[DOWN][i][2];
    }
    for (int i = 0; i < 3; i++) {
        faces[DOWN][i][2] = faces[BACK][2 - i][0];
    }
    for (int i = 0; i < 3; i++) {
        faces[BACK][2 - i][0] = temp[i];
    }
}

// Rotate left face clockwise (L move)
void RubikCube::rotateL() {
    rotateFaceClockwise(LEFT);
    
    // Rotate adjacent edge pieces
    int temp[3];
    for (int i = 0; i < 3; i++) {
        temp[i] = faces[UP][i][0];
    }
    for (int i = 0; i < 3; i++) {
        faces[UP][i][0] = faces[BACK][2 - i][2];
    }
    for (int i = 0; i < 3; i++) {
        faces[BACK][2 - i][2] = faces[DOWN][i][0];
    }
    for (int i = 0; i < 3; i++) {
        faces[DOWN][i][0] = faces[FRONT][i][0];
    }
    for (int i = 0; i < 3; i++) {
        faces[FRONT][i][0] = temp[i];
    }
}

// Rotate up face clockwise (U move)
void RubikCube::rotateU() {
    rotateFaceClockwise(UP);
    
    // Rotate adjacent edge pieces
    int temp[3];
    for (int i = 0; i < 3; i++) {
        temp[i] = faces[FRONT][0][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[FRONT][0][i] = faces[RIGHT][0][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[RIGHT][0][i] = faces[BACK][0][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[BACK][0][i] = faces[LEFT][0][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[LEFT][0][i] = temp[i];
    }
}

// Rotate down face clockwise (D move)
void RubikCube::rotateD() {
    rotateFaceClockwise(DOWN);
    
    // Rotate adjacent edge pieces
    int temp[3];
    for (int i = 0; i < 3; i++) {
        temp[i] = faces[FRONT][2][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[FRONT][2][i] = faces[LEFT][2][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[LEFT][2][i] = faces[BACK][2][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[BACK][2][i] = faces[RIGHT][2][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[RIGHT][2][i] = temp[i];
    }
}

// Rotate front face clockwise (F move)
void RubikCube::rotateF() {
    rotateFaceClockwise(FRONT);
    
    // Rotate adjacent edge pieces
    int temp[3];
    for (int i = 0; i < 3; i++) {
        temp[i] = faces[UP][2][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[UP][2][i] = faces[LEFT][2 - i][2];
    }
    for (int i = 0; i < 3; i++) {
        faces[LEFT][2 - i][2] = faces[DOWN][0][2 - i];
    }
    for (int i = 0; i < 3; i++) {
        faces[DOWN][0][2 - i] = faces[RIGHT][i][0];
    }
    for (int i = 0; i < 3; i++) {
        faces[RIGHT][i][0] = temp[i];
    }
}

// Rotate back face clockwise (B move)
void RubikCube::rotateB() {
    rotateFaceClockwise(BACK);
    
    // Rotate adjacent edge pieces
    int temp[3];
    for (int i = 0; i < 3; i++) {
        temp[i] = faces[UP][0][i];
    }
    for (int i = 0; i < 3; i++) {
        faces[UP][0][i] = faces[RIGHT][i][2];
    }
    for (int i = 0; i < 3; i++) {
        faces[RIGHT][i][2] = faces[DOWN][2][2 - i];
    }
    for (int i = 0; i < 3; i++) {
        faces[DOWN][2][2 - i] = faces[LEFT][2 - i][0];
    }
    for (int i = 0; i < 3; i++) {
        faces[LEFT][2 - i][0] = temp[i];
    }
}

// Rotate right face counter-clockwise (R' move)
void RubikCube::rotateRPrime() {
    rotateR(); rotateR(); rotateR();  // Three clockwise = one counter-clockwise
}

// Rotate left face counter-clockwise (L' move)
void RubikCube::rotateLPrime() {
    rotateL(); rotateL(); rotateL();
}

// Rotate up face counter-clockwise (U' move)
void RubikCube::rotateUPrime() {
    rotateU(); rotateU(); rotateU();
}

// Rotate down face counter-clockwise (D' move)
void RubikCube::rotateDPrime() {
    rotateD(); rotateD(); rotateD();
}

// Rotate front face counter-clockwise (F' move)
void RubikCube::rotateFPrime() {
    rotateF(); rotateF(); rotateF();
}

// Rotate back face counter-clockwise (B' move)
void RubikCube::rotateBPrime() {
    rotateB(); rotateB(); rotateB();
}

// Apply move from standard notation (e.g., "R", "R'", "U")
bool RubikCube::applyMove(const std::string& move) {
    if (move == "R") {
        rotateR();
        return true;
    } else if (move == "R'" || move == "R'") {
        rotateRPrime();
        return true;
    } else if (move == "L") {
        rotateL();
        return true;
    } else if (move == "L'" || move == "L'") {
        rotateLPrime();
        return true;
    } else if (move == "U") {
        rotateU();
        return true;
    } else if (move == "U'" || move == "U'") {
        rotateUPrime();
        return true;
    } else if (move == "D") {
        rotateD();
        return true;
    } else if (move == "D'" || move == "D'") {
        rotateDPrime();
        return true;
    } else if (move == "F") {
        rotateF();
        return true;
    } else if (move == "F'" || move == "F'") {
        rotateFPrime();
        return true;
    } else if (move == "B") {
        rotateB();
        return true;
    } else if (move == "B'" || move == "B'") {
        rotateBPrime();
        return true;
    }
    return false;
}

// Scramble cube with random moves
void RubikCube::scramble(int numMoves) {
    std::vector<std::string> moves = {"R", "R'", "L", "L'", "U", "U'", "D", "D'", "F", "F'", "B", "B'"};
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> dist(0, moves.size() - 1);
    
    for (int i = 0; i < numMoves; i++) {
        applyMove(moves[dist(rng)]);
    }
}

// Check if cube is in solved state
bool RubikCube::isSolved() const {
    int faceColors[] = {RED, ORANGE, WHITE, YELLOW, GREEN, BLUE};
    for (int face = 0; face < 6; face++) {
        int expectedColor = faceColors[face];
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (faces[face][row][col] != expectedColor) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Get color at specific position
int RubikCube::getColor(int face, int row, int col) const {
    return faces[face][row][col];
}

// Get all faces (for rendering)
const std::vector<std::vector<std::vector<int>>>& RubikCube::getFaces() const {
    return faces;
}

