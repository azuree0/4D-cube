// Rubik's Cube Logic Header
// 3x3x3 cube for inner tesseract

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

class RubikCube {
private:
    std::vector<std::vector<std::vector<int>>> faces;
    void rotateFaceClockwise(int face);
    void rotateFaceCounterClockwise(int face);

public:
    RubikCube();
    void reset();
    void rotateR(); void rotateL(); void rotateU(); void rotateD(); void rotateF(); void rotateB();
    void rotateRPrime(); void rotateLPrime(); void rotateUPrime(); void rotateDPrime(); void rotateFPrime(); void rotateBPrime();
    bool applyMove(const std::string& move);
    void scramble(int numMoves = 25);
    bool isSolved() const;
    int getColor(int face, int row, int col) const;
    const std::vector<std::vector<std::vector<int>>>& getFaces() const;
};

#endif // RUBIK_CUBE_H
