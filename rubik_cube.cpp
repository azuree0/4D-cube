// Rubik's Cube Implementation - 3x3x3 inner cube

#include "rubik_cube.h"
#include <algorithm>
#include <random>
#include <ctime>

RubikCube::RubikCube() {
    faces.resize(6);
    int faceColors[] = {RED, ORANGE, WHITE, YELLOW, GREEN, BLUE};
    for (int i = 0; i < 6; i++) {
        faces[i].resize(3);
        for (int j = 0; j < 3; j++)
            faces[i][j].resize(3, faceColors[i]);
    }
}

void RubikCube::reset() {
    int faceColors[] = {RED, ORANGE, WHITE, YELLOW, GREEN, BLUE};
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                faces[i][j][k] = faceColors[i];
}

void RubikCube::rotateFaceClockwise(int face) {
    std::vector<std::vector<int>> temp(3, std::vector<int>(3));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            temp[j][2 - i] = faces[face][i][j];
    faces[face] = temp;
}

void RubikCube::rotateFaceCounterClockwise(int face) {
    std::vector<std::vector<int>> temp(3, std::vector<int>(3));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            temp[2 - j][i] = faces[face][i][j];
    faces[face] = temp;
}

void RubikCube::rotateR() {
    rotateFaceClockwise(RIGHT);
    int temp[3];
    for (int i = 0; i < 3; i++) temp[i] = faces[UP][i][2];
    for (int i = 0; i < 3; i++) faces[UP][i][2] = faces[FRONT][i][2];
    for (int i = 0; i < 3; i++) faces[FRONT][i][2] = faces[DOWN][i][2];
    for (int i = 0; i < 3; i++) faces[DOWN][i][2] = faces[BACK][2 - i][0];
    for (int i = 0; i < 3; i++) faces[BACK][2 - i][0] = temp[i];
}

void RubikCube::rotateL() {
    rotateFaceClockwise(LEFT);
    int temp[3];
    for (int i = 0; i < 3; i++) temp[i] = faces[UP][i][0];
    for (int i = 0; i < 3; i++) faces[UP][i][0] = faces[BACK][2 - i][2];
    for (int i = 0; i < 3; i++) faces[BACK][2 - i][2] = faces[DOWN][i][0];
    for (int i = 0; i < 3; i++) faces[DOWN][i][0] = faces[FRONT][i][0];
    for (int i = 0; i < 3; i++) faces[FRONT][i][0] = temp[i];
}

void RubikCube::rotateU() {
    rotateFaceClockwise(UP);
    int temp[3];
    for (int i = 0; i < 3; i++) temp[i] = faces[FRONT][0][i];
    for (int i = 0; i < 3; i++) faces[FRONT][0][i] = faces[RIGHT][0][i];
    for (int i = 0; i < 3; i++) faces[RIGHT][0][i] = faces[BACK][0][i];
    for (int i = 0; i < 3; i++) faces[BACK][0][i] = faces[LEFT][0][i];
    for (int i = 0; i < 3; i++) faces[LEFT][0][i] = temp[i];
}

void RubikCube::rotateD() {
    rotateFaceClockwise(DOWN);
    int temp[3];
    for (int i = 0; i < 3; i++) temp[i] = faces[FRONT][2][i];
    for (int i = 0; i < 3; i++) faces[FRONT][2][i] = faces[LEFT][2][i];
    for (int i = 0; i < 3; i++) faces[LEFT][2][i] = faces[BACK][2][i];
    for (int i = 0; i < 3; i++) faces[BACK][2][i] = faces[RIGHT][2][i];
    for (int i = 0; i < 3; i++) faces[RIGHT][2][i] = temp[i];
}

void RubikCube::rotateF() {
    rotateFaceClockwise(FRONT);
    int temp[3];
    for (int i = 0; i < 3; i++) temp[i] = faces[UP][2][i];
    for (int i = 0; i < 3; i++) faces[UP][2][i] = faces[LEFT][2 - i][2];
    for (int i = 0; i < 3; i++) faces[LEFT][2 - i][2] = faces[DOWN][0][2 - i];
    for (int i = 0; i < 3; i++) faces[DOWN][0][2 - i] = faces[RIGHT][i][0];
    for (int i = 0; i < 3; i++) faces[RIGHT][i][0] = temp[i];
}

void RubikCube::rotateB() {
    rotateFaceClockwise(BACK);
    int temp[3];
    for (int i = 0; i < 3; i++) temp[i] = faces[UP][0][i];
    for (int i = 0; i < 3; i++) faces[UP][0][i] = faces[RIGHT][i][2];
    for (int i = 0; i < 3; i++) faces[RIGHT][i][2] = faces[DOWN][2][2 - i];
    for (int i = 0; i < 3; i++) faces[DOWN][2][2 - i] = faces[LEFT][2 - i][0];
    for (int i = 0; i < 3; i++) faces[LEFT][2 - i][0] = temp[i];
}

void RubikCube::rotateRPrime() { rotateR(); rotateR(); rotateR(); }
void RubikCube::rotateLPrime() { rotateL(); rotateL(); rotateL(); }
void RubikCube::rotateUPrime() { rotateU(); rotateU(); rotateU(); }
void RubikCube::rotateDPrime() { rotateD(); rotateD(); rotateD(); }
void RubikCube::rotateFPrime() { rotateF(); rotateF(); rotateF(); }
void RubikCube::rotateBPrime() { rotateB(); rotateB(); rotateB(); }

bool RubikCube::applyMove(const std::string& move) {
    if (move == "R") { rotateR(); return true; }
    if (move == "R'") { rotateRPrime(); return true; }
    if (move == "L") { rotateL(); return true; }
    if (move == "L'") { rotateLPrime(); return true; }
    if (move == "U") { rotateU(); return true; }
    if (move == "U'") { rotateUPrime(); return true; }
    if (move == "D") { rotateD(); return true; }
    if (move == "D'") { rotateDPrime(); return true; }
    if (move == "F") { rotateF(); return true; }
    if (move == "F'") { rotateFPrime(); return true; }
    if (move == "B") { rotateB(); return true; }
    if (move == "B'") { rotateBPrime(); return true; }
    return false;
}

void RubikCube::scramble(int numMoves) {
    const char* moves[] = {"R", "R'", "L", "L'", "U", "U'", "D", "D'", "F", "F'", "B", "B'"};
    std::mt19937 rng(42u);
    std::uniform_int_distribution<int> dist(0, 11);
    for (int i = 0; i < numMoves; i++) {
        applyMove(moves[dist(rng)]);
    }
}

bool RubikCube::isSolved() const {
    int faceColors[] = {RED, ORANGE, WHITE, YELLOW, GREEN, BLUE};
    for (int face = 0; face < 6; face++)
        for (int row = 0; row < 3; row++)
            for (int col = 0; col < 3; col++)
                if (faces[face][row][col] != faceColors[face]) return false;
    return true;
}

int RubikCube::getColor(int face, int row, int col) const {
    return faces[face][row][col];
}

const std::vector<std::vector<std::vector<int>>>& RubikCube::getFaces() const {
    return faces;
}
