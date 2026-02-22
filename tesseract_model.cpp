// Tesseract Puzzle Implementation
// 2x2x2x2 state and plane-based slice rotations

#include "tesseract_model.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <cstring>

// Vertex index from grid coords (each 0 or 1)
int TesseractPuzzle::vertexIndex(int ix, int iy, int iz, int iw) const {
    return ix * 8 + iy * 4 + iz * 2 + iw;
}

void TesseractPuzzle::initSolved() {
    // Each vertex gets colors for the 4 cells it touches (where coord = 1)
    // Slot 0 = +X, 1 = +Y, 2 = +Z, 3 = +W. For vertex (ix,iy,iz,iw):
    // slot 0 used when ix=1 (touches +X cell), slot 1 when iy=1, etc.
    static const int solved[16][4] = {
        {C_X_NEG, C_Y_NEG, C_Z_NEG, C_W_NEG}, // 0: (0,0,0,0) - no +axis
        {C_X_NEG, C_Y_NEG, C_Z_NEG, C_W_POS}, // 1
        {C_X_NEG, C_Y_NEG, C_Z_POS, C_W_NEG}, // 2
        {C_X_NEG, C_Y_NEG, C_Z_POS, C_W_POS}, // 3
        {C_X_NEG, C_Y_POS, C_Z_NEG, C_W_NEG}, // 4
        {C_X_NEG, C_Y_POS, C_Z_NEG, C_W_POS}, // 5
        {C_X_NEG, C_Y_POS, C_Z_POS, C_W_NEG}, // 6
        {C_X_NEG, C_Y_POS, C_Z_POS, C_W_POS}, // 7
        {C_X_POS, C_Y_NEG, C_Z_NEG, C_W_NEG}, // 8
        {C_X_POS, C_Y_NEG, C_Z_NEG, C_W_POS}, // 9
        {C_X_POS, C_Y_NEG, C_Z_POS, C_W_NEG}, // 10
        {C_X_POS, C_Y_NEG, C_Z_POS, C_W_POS}, // 11
        {C_X_POS, C_Y_POS, C_Z_NEG, C_W_NEG}, // 12
        {C_X_POS, C_Y_POS, C_Z_NEG, C_W_POS}, // 13
        {C_X_POS, C_Y_POS, C_Z_POS, C_W_NEG}, // 14
        {C_X_POS, C_Y_POS, C_Z_POS, C_W_POS}, // 15
    };
    for (int i = 0; i < 16; i++)
        for (int s = 0; s < 4; s++)
            vertices_[i].colors[s] = solved[i][s];
}

TesseractPuzzle::TesseractPuzzle() {
    initSolved();
}

void TesseractPuzzle::reset() {
    initSolved();
}

// Get 4 vertex indices for (plane, layer). Layer 0..3.
// Returns indices in order: (0,0), (1,0), (1,1), (0,1) for the 2D sub-coords.
static void getLayerIndices(int plane, int layer, int indices[4]) {
    int a = layer / 2;
    int b = layer % 2;
    switch (plane) {
        case PLANE_XY: { int iz = a, iw = b; indices[0] = iz*2+iw; indices[1] = 8+iz*2+iw; indices[2] = 12+iz*2+iw; indices[3] = 4+iz*2+iw; break; }
        case PLANE_XZ: { int iy = a, iw = b; indices[0] = iy*4+iw; indices[1] = 8+iy*4+iw; indices[2] = 10+iy*4+iw; indices[3] = iy*4+2+iw; break; }
        case PLANE_XW: { int iy = a, iz = b; indices[0] = iy*4+iz*2; indices[1] = 8+iy*4+iz*2; indices[2] = 8+iy*4+iz*2+1; indices[3] = iy*4+iz*2+1; break; }
        case PLANE_YZ: { int ix = a, iw = b; indices[0] = ix*8+iw; indices[1] = ix*8+4+iw; indices[2] = ix*8+6+iw; indices[3] = ix*8+2+iw; break; }
        case PLANE_YW: { int ix = a, iz = b; indices[0] = ix*8+iz*2; indices[1] = ix*8+4+iz*2; indices[2] = ix*8+6+iz*2; indices[3] = ix*8+2+iz*2; break; }
        case PLANE_ZW: { int ix = a, iy = b; indices[0] = ix*8+iy*4; indices[1] = ix*8+iy*4+2; indices[2] = ix*8+iy*4+3; indices[3] = ix*8+iy*4+1; break; }
        default: indices[0]=0; indices[1]=1; indices[2]=2; indices[3]=3; break;
    }
}

// Slots to swap for each plane (0,1) or (0,2) or (0,3) or (1,2) or (1,3) or (2,3)
static void getSlotSwap(int plane, int& s0, int& s1) {
    switch (plane) {
        case PLANE_XY: s0=0; s1=1; break;
        case PLANE_XZ: s0=0; s1=2; break;
        case PLANE_XW: s0=0; s1=3; break;
        case PLANE_YZ: s0=1; s1=2; break;
        case PLANE_YW: s0=1; s1=3; break;
        case PLANE_ZW: s0=2; s1=3; break;
        default: s0=0; s1=1; break;
    }
}

void TesseractPuzzle::rotateSlice(int plane, int layer, bool clockwise) {
    int idx[4];
    getLayerIndices(plane, layer, idx);
    int s0, s1;
    getSlotSwap(plane, s0, s1);

    Vertex4D temp[4];
    for (int i = 0; i < 4; i++) {
        temp[i] = vertices_[idx[i]];
        std::swap(temp[i].colors[s0], temp[i].colors[s1]);
    }
    if (clockwise) {
        vertices_[idx[1]] = temp[0];
        vertices_[idx[2]] = temp[1];
        vertices_[idx[3]] = temp[2];
        vertices_[idx[0]] = temp[3];
    } else {
        vertices_[idx[0]] = temp[1];
        vertices_[idx[1]] = temp[2];
        vertices_[idx[2]] = temp[3];
        vertices_[idx[3]] = temp[0];
    }
}

// Move notation: "XY0", "XY0'", "XZ1", etc. Plane name + layer (0-3) + optional '
bool TesseractPuzzle::applyMove(const std::string& move) {
    if (move.size() < 3) return false;
    int plane = -1;
    if (move[0] == 'X' && move[1] == 'Y') plane = PLANE_XY;
    else if (move[0] == 'X' && move[1] == 'Z') plane = PLANE_XZ;
    else if (move[0] == 'X' && move[1] == 'W') plane = PLANE_XW;
    else if (move[0] == 'Y' && move[1] == 'Z') plane = PLANE_YZ;
    else if (move[0] == 'Y' && move[1] == 'W') plane = PLANE_YW;
    else if (move[0] == 'Z' && move[1] == 'W') plane = PLANE_ZW;
    if (plane < 0) return false;
    int layer = move[2] - '0';
    if (layer < 0 || layer > 3) return false;
    bool cw = true;
    if (move.size() >= 4 && (move[3] == '\'' || move[3] == '`')) cw = false;
    rotateSlice(plane, layer, cw);
    return true;
}

void TesseractPuzzle::scramble(int numMoves) {
    static const char* planes[] = {"XY","XZ","XW","YZ","YW","ZW"};
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<int> pdist(0, 5);
    std::uniform_int_distribution<int> ldist(0, 3);
    std::uniform_int_distribution<int> ddist(0, 1);
    for (int i = 0; i < numMoves; i++) {
        int pl = pdist(rng);
        int lay = ldist(rng);
        bool cw = ddist(rng) == 0;
        std::string m = std::string(planes[pl]) + char('0' + lay);
        if (!cw) m += "'";
        applyMove(m);
    }
}

bool TesseractPuzzle::isSolved() const {
    TesseractPuzzle solved;
    for (int i = 0; i < 16; i++) {
        for (int s = 0; s < 4; s++) {
            if (vertices_[i].colors[s] != solved.vertices_[i].colors[s])
                return false;
        }
    }
    return true;
}

const Vertex4D& TesseractPuzzle::getVertex(int ix, int iy, int iz, int iw) const {
    return vertices_[vertexIndex(ix, iy, iz, iw)];
}

void TesseractPuzzle::getAllVertices(std::vector<Vertex4D>& out) const {
    out.resize(16);
    for (int i = 0; i < 16; i++) out[i] = vertices_[i];
}

bool TesseractPuzzle::isVertexInSlice(int vertexIndex, int plane, int layer) {
    int idx[4];
    getLayerIndices(plane, layer, idx);
    for (int i = 0; i < 4; i++)
        if (idx[i] == vertexIndex) return true;
    return false;
}
