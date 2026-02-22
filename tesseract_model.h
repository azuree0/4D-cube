// Tesseract Puzzle Model
// 2x2x2x2 tesseract with plane-based rotations (XY, XZ, XW, YZ, YW, ZW)

#ifndef TESSERACT_MODEL_H
#define TESSERACT_MODEL_H

#include <vector>
#include <string>

// Cell colors: 0..7 for each of 8 cubic cells (+X,-X,+Y,-Y,+Z,-Z,+W,-W)
enum CellColor {
    C_X_POS = 0,  // +X cell
    C_X_NEG = 1,
    C_Y_POS = 2,
    C_Y_NEG = 3,
    C_Z_POS = 4,
    C_Z_NEG = 5,
    C_W_POS = 6,
    C_W_NEG = 7
};

// Rotation planes: XY=0, XZ=1, XW=2, YZ=3, YW=4, ZW=5
enum RotationPlane {
    PLANE_XY = 0,
    PLANE_XZ = 1,
    PLANE_XW = 2,
    PLANE_YZ = 3,
    PLANE_YW = 4,
    PLANE_ZW = 5
};

// 4D vertex: 4 sticker slots (one per axis direction at that corner)
struct Vertex4D {
    int colors[4];  // slots for +axis0, +axis1, +axis2, +axis3 (depends on vertex position)
};

// Tesseract puzzle state: 16 vertices, plane-based moves
class TesseractPuzzle {
public:
    TesseractPuzzle();

    void reset();
    void rotateSlice(int plane, int layer, bool clockwise);
    bool applyMove(const std::string& move);
    void scramble(int numMoves = 30);
    bool isSolved() const;

    // For rendering: get vertex at grid position (ix,iy,iz,iw) each in {0,1}
    const Vertex4D& getVertex(int ix, int iy, int iz, int iw) const;
    void getAllVertices(std::vector<Vertex4D>& out) const;

    // Check if vertex index (0..15) is in the given plane/layer (for animation)
    static bool isVertexInSlice(int vertexIndex, int plane, int layer);

private:
    Vertex4D vertices_[16];

    int vertexIndex(int ix, int iy, int iz, int iw) const;
    void initSolved();
};

#endif // TESSERACT_MODEL_H
