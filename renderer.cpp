// 4D Tesseract OpenGL Renderer Implementation
// 4D projection, edge/vertex drawing, camera control

#include "renderer.h"
#include "projection_4d.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cfloat>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// 4D vertex positions for 2x2x2x2 tesseract (scale 1.0)
static void getVertexPos(int ix, int iy, int iz, int iw, Vec4& out) {
    float x = (ix == 0) ? -1.0f : 1.0f;
    float y = (iy == 0) ? -1.0f : 1.0f;
    float z = (iz == 0) ? -1.0f : 1.0f;
    float w = (iw == 0) ? -1.0f : 1.0f;
    out = Vec4(x, y, z, w);
}

// 32 edges: pairs of vertex indices
static const int EDGES[32][2] = {
    {0,1},{0,2},{0,4},{0,8},{1,3},{1,5},{1,9},{2,3},{2,6},{2,10},{3,7},{3,11},
    {4,5},{4,6},{4,12},{5,7},{5,13},{6,7},{6,14},{7,15},{8,9},{8,10},{8,12},{9,11},{9,13},
    {10,11},{10,14},{11,15},{12,13},{12,14},{13,15},{14,15}
};

static int vindex(int ix, int iy, int iz, int iw) {
    return ix*8 + iy*4 + iz*2 + iw;
}

// Build 3D rotation for Rubik face (90°). Plane: XY=0, XZ=1, YZ=3.
static Mat4x4 rubikFaceRotation(int face, bool clockwise) {
    int plane4d = -1;
    float angle = clockwise ? 90.0f : -90.0f;
    switch (face) {
        case 0: plane4d = 3; break;  // R: YZ
        case 1: plane4d = 3; angle = -angle; break;  // L
        case 2: plane4d = 1; break;  // U: XZ
        case 3: plane4d = 1; angle = -angle; break;  // D
        case 4: plane4d = 0; break;  // F: XY
        case 5: plane4d = 0; angle = -angle; break;  // B
        default: return Mat4x4::identity();
    }
    return rotate4D(plane4d, angle);
}

// Apply in-progress Rubik animation rotation to vertex if in slice
static Vec4 applyRubikAnimToVertex(const Vec4& p, int ix, int iy, int iz, int iw, const RubikAnimState& anim) {
    if (!anim.isAnimating || anim.face < 0) return p;
    bool inSlice = false;
    int plane4d = -1;
    float angle = anim.clockwise ? anim.currentAngle : -anim.currentAngle;
    switch (anim.face) {
        case 0: if (ix == 1) { inSlice = true; plane4d = 3; } break;
        case 1: if (ix == 0) { inSlice = true; plane4d = 3; angle = -angle; } break;
        case 2: if (iy == 1) { inSlice = true; plane4d = 1; } break;
        case 3: if (iy == 0) { inSlice = true; plane4d = 1; angle = -angle; } break;
        case 4: if (iz == 1) { inSlice = true; plane4d = 0; } break;
        case 5: if (iz == 0) { inSlice = true; plane4d = 0; angle = -angle; } break;
        default: break;
    }
    if (!inSlice || plane4d < 0) return p;
    Mat4x4 rot = rotate4D(plane4d, angle);
    return matMul(rot, p);
}

Renderer::Renderer() {
    cameraAngleX = 30.0f;
    cameraAngleY = 45.0f;
    cameraDistance = 8.0f;
    viewAngleW_ = 15.0f;
    wDistance_ = 4.0f;
    for (int ix = 0; ix < 2; ix++)
        for (int iy = 0; iy < 2; iy++)
            for (int iz = 0; iz < 2; iz++)
                for (int iw = 0; iw < 2; iw++) {
                    int idx = vindex(ix, iy, iz, iw);
                    getVertexPos(ix, iy, iz, iw, outerPositions_[idx]);
                }
}

void Renderer::drawStars() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    std::srand(42);
    for (int i = 0; i < 150; i++) {
        float theta = (float)(std::rand() % 628) / 100.0f;
        float phi = (float)(std::rand() % 314) / 100.0f;
        float r = 50.0f;
        glVertex3f(r * sin(phi) * cos(theta), r * sin(phi) * sin(theta), r * cos(phi));
    }
    glPointSize(3.0f);
    glColor3f(1.0f, 1.0f, 0.9f);
    for (int i = 0; i < 15; i++) {
        float theta = (float)(std::rand() % 628) / 100.0f;
        float phi = (float)(std::rand() % 314) / 100.0f;
        float r = 50.0f;
        glVertex3f(r * sin(phi) * cos(theta), r * sin(phi) * sin(theta), r * cos(phi));
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    GLfloat lightPos[] = {5.0f, 5.0f, 5.0f, 1.0f};
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat lightSpecular[] = {1.5f, 1.5f, 1.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Renderer::setColor(int cellColor) {
    switch (cellColor) {
        case C_X_POS: glColor3f(1.0f, 0.0f, 0.0f); break;
        case C_X_NEG: glColor3f(1.0f, 0.5f, 0.0f); break;
        case C_Y_POS: glColor3f(1.0f, 1.0f, 1.0f); break;
        case C_Y_NEG: glColor3f(1.0f, 1.0f, 0.0f); break;
        case C_Z_POS: glColor3f(0.0f, 1.0f, 0.0f); break;
        case C_Z_NEG: glColor3f(0.0f, 0.0f, 1.0f); break;
        case C_W_POS: glColor3f(1.0f, 0.0f, 1.0f); break;
        case C_W_NEG: glColor3f(0.0f, 1.0f, 1.0f); break;
        default: glColor3f(0.2f, 0.2f, 0.2f); break;
    }
}

void Renderer::setColorTranslucent(int cellColor, float alpha) {
    switch (cellColor) {
        case C_X_POS: glColor4f(1.0f, 0.0f, 0.0f, alpha); break;
        case C_X_NEG: glColor4f(1.0f, 0.5f, 0.0f, alpha); break;
        case C_Y_POS: glColor4f(1.0f, 1.0f, 1.0f, alpha); break;
        case C_Y_NEG: glColor4f(1.0f, 1.0f, 0.0f, alpha); break;
        case C_Z_POS: glColor4f(0.0f, 1.0f, 0.0f, alpha); break;
        case C_Z_NEG: glColor4f(0.0f, 0.0f, 1.0f, alpha); break;
        case C_W_POS: glColor4f(1.0f, 0.0f, 1.0f, alpha); break;
        case C_W_NEG: glColor4f(0.0f, 1.0f, 1.0f, alpha); break;
        default: glColor4f(0.2f, 0.2f, 0.2f, alpha); break;
    }
}

void Renderer::setColorRubik(int faceColor) {
    switch (faceColor) {
        case WHITE: glColor3f(1.0f, 1.0f, 1.0f); break;
        case YELLOW: glColor3f(1.0f, 1.0f, 0.0f); break;
        case RED: glColor3f(1.0f, 0.0f, 0.0f); break;
        case ORANGE: glColor3f(1.0f, 0.5f, 0.0f); break;
        case GREEN: glColor3f(0.0f, 1.0f, 0.0f); break;
        case BLUE: glColor3f(0.0f, 0.0f, 1.0f); break;
        default: glColor3f(0.2f, 0.2f, 0.2f); break;
    }
}

void Renderer::drawFaceRubik(float x, float y, float z, float size, int faceIndex, int faceColor) {
    float s = size / 2.0f;
    float offset = 0.01f;
    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess[] = {128.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
    setColorRubik(faceColor);
    glFlush();  // Ensure no pending commands before glBegin (avoids crash on some drivers)
    glBegin(GL_TRIANGLES);  // GL_QUADS can crash on some drivers; use triangles
    switch (faceIndex) {
        case 0:
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(x + s + offset, y - s, z - s);
            glVertex3f(x + s + offset, y + s, z - s);
            glVertex3f(x + s + offset, y + s, z + s);
            glVertex3f(x + s + offset, y - s, z - s);
            glVertex3f(x + s + offset, y + s, z + s);
            glVertex3f(x + s + offset, y - s, z + s);
            break;
        case 1:
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(x - s - offset, y - s, z + s);
            glVertex3f(x - s - offset, y + s, z + s);
            glVertex3f(x - s - offset, y + s, z - s);
            glVertex3f(x - s - offset, y - s, z + s);
            glVertex3f(x - s - offset, y + s, z - s);
            glVertex3f(x - s - offset, y - s, z - s);
            break;
        case 2:
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(x - s, y + s + offset, z - s);
            glVertex3f(x + s, y + s + offset, z - s);
            glVertex3f(x + s, y + s + offset, z + s);
            glVertex3f(x - s, y + s + offset, z - s);
            glVertex3f(x + s, y + s + offset, z + s);
            glVertex3f(x - s, y + s + offset, z + s);
            break;
        case 3:
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(x - s, y - s - offset, z + s);
            glVertex3f(x + s, y - s - offset, z + s);
            glVertex3f(x + s, y - s - offset, z - s);
            glVertex3f(x - s, y - s - offset, z + s);
            glVertex3f(x + s, y - s - offset, z - s);
            glVertex3f(x - s, y - s - offset, z - s);
            break;
        case 4:
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(x - s, y - s, z + s + offset);
            glVertex3f(x - s, y + s, z + s + offset);
            glVertex3f(x + s, y + s, z + s + offset);
            glVertex3f(x - s, y - s, z + s + offset);
            glVertex3f(x + s, y + s, z + s + offset);
            glVertex3f(x + s, y - s, z + s + offset);
            break;
        case 5:
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(x + s, y - s, z - s - offset);
            glVertex3f(x + s, y + s, z - s - offset);
            glVertex3f(x - s, y + s, z - s - offset);
            glVertex3f(x + s, y - s, z - s - offset);
            glVertex3f(x - s, y + s, z - s - offset);
            glVertex3f(x - s, y - s, z - s - offset);
            break;
    }
    glEnd();
}

void Renderer::drawCubieRubik(float x, float y, float z, float size, const RubikCube& cube, int cx, int cy, int cz, const RubikAnimState& anim) {
    const auto& faces = cube.getFaces();
    bool isRotating = false;
    float rotationAngle = 0.0f;
    int rotationAxis = 0;
    float faceCenterX = 0.0f, faceCenterY = 0.0f, faceCenterZ = 0.0f;

    if (anim.isAnimating) {
        switch (anim.face) {
            case RIGHT:
                if (cx == 1) { isRotating = true; rotationAxis = 0; rotationAngle = anim.currentAngle; faceCenterX = 1.0f; }
                break;
            case LEFT:
                if (cx == -1) { isRotating = true; rotationAxis = 0; rotationAngle = -anim.currentAngle; faceCenterX = -1.0f; }
                break;
            case UP:
                if (cy == 1) { isRotating = true; rotationAxis = 1; rotationAngle = anim.currentAngle; faceCenterY = 1.0f; }
                break;
            case DOWN:
                if (cy == -1) { isRotating = true; rotationAxis = 1; rotationAngle = -anim.currentAngle; faceCenterY = -1.0f; }
                break;
            case FRONT:
                if (cz == 1) { isRotating = true; rotationAxis = 2; rotationAngle = anim.currentAngle; faceCenterZ = 1.0f; }
                break;
            case BACK:
                if (cz == -1) { isRotating = true; rotationAxis = 2; rotationAngle = -anim.currentAngle; faceCenterZ = -1.0f; }
                break;
        }
    }

    if (isRotating) {
        glPushMatrix();
        glTranslatef(faceCenterX, faceCenterY, faceCenterZ);
        switch (rotationAxis) {
            case 0: glRotatef(rotationAngle, 1.0f, 0.0f, 0.0f); break;
            case 1: glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); break;
            case 2: glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f); break;
        }
        glTranslatef(-faceCenterX, -faceCenterY, -faceCenterZ);
        glTranslatef(x, y, z);
    } else {
        glPushMatrix();
        glTranslatef(x, y, z);
    }

    auto safeColor = [&faces](int f, int r, int c) -> int {
        if (f < 0 || f >= 6 || r < 0 || r > 2 || c < 0 || c > 2) return 0;
        return faces[f][r][c];
    };
    int row = 1 - cy, col = 1 - cz;
    drawFaceRubik(0, 0, 0, size, 0, safeColor(RIGHT, row, col));
    row = 1 - cy; col = cz + 1;
    drawFaceRubik(0, 0, 0, size, 1, safeColor(LEFT, row, col));
    row = cz + 1; col = cx + 1;
    drawFaceRubik(0, 0, 0, size, 2, safeColor(UP, row, col));
    row = 1 - cz; col = cx + 1;
    drawFaceRubik(0, 0, 0, size, 3, safeColor(DOWN, row, col));
    row = 1 - cy; col = cx + 1;
    drawFaceRubik(0, 0, 0, size, 4, safeColor(FRONT, row, col));
    row = 1 - cy; col = 1 - cx;
    drawFaceRubik(0, 0, 0, size, 5, safeColor(BACK, row, col));
    drawCube(0, 0, 0, size);
    glPopMatrix();
}

// Cube geometry from Rubik 1974 AD: 6 faces with offset, 12 edges
void Renderer::drawFace(float x, float y, float z, float size, int faceIndex, int color) {
    float s = size / 2.0f;
    float offset = 0.01f;
    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess[] = {128.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
    glBegin(GL_QUADS);
    setColor(color);
    switch (faceIndex) {
        case 0: // Right (+X)
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(x + s + offset, y - s, z - s);
            glVertex3f(x + s + offset, y + s, z - s);
            glVertex3f(x + s + offset, y + s, z + s);
            glVertex3f(x + s + offset, y - s, z + s);
            break;
        case 1: // Left (-X)
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(x - s - offset, y - s, z + s);
            glVertex3f(x - s - offset, y + s, z + s);
            glVertex3f(x - s - offset, y + s, z - s);
            glVertex3f(x - s - offset, y - s, z - s);
            break;
        case 2: // Up (+Y)
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(x - s, y + s + offset, z - s);
            glVertex3f(x + s, y + s + offset, z - s);
            glVertex3f(x + s, y + s + offset, z + s);
            glVertex3f(x - s, y + s + offset, z + s);
            break;
        case 3: // Down (-Y)
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(x - s, y - s - offset, z + s);
            glVertex3f(x + s, y - s - offset, z + s);
            glVertex3f(x + s, y - s - offset, z - s);
            glVertex3f(x - s, y - s - offset, z - s);
            break;
        case 4: // Front (+Z)
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(x - s, y - s, z + s + offset);
            glVertex3f(x - s, y + s, z + s + offset);
            glVertex3f(x + s, y + s, z + s + offset);
            glVertex3f(x + s, y - s, z + s + offset);
            break;
        case 5: // Back (-Z)
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(x + s, y - s, z - s - offset);
            glVertex3f(x + s, y + s, z - s - offset);
            glVertex3f(x - s, y + s, z - s - offset);
            glVertex3f(x - s, y - s, z - s - offset);
            break;
    }
    glEnd();
}

void Renderer::drawFaceTranslucent(float x, float y, float z, float size, int faceIndex, int color, float alpha) {
    float s = size / 2.0f;
    float offset = 0.01f;
    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess[] = {128.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
    glBegin(GL_QUADS);
    setColorTranslucent(color, alpha);
    switch (faceIndex) {
        case 0:
            glNormal3f(1.0f, 0.0f, 0.0f);
            glVertex3f(x + s + offset, y - s, z - s);
            glVertex3f(x + s + offset, y + s, z - s);
            glVertex3f(x + s + offset, y + s, z + s);
            glVertex3f(x + s + offset, y - s, z + s);
            break;
        case 1:
            glNormal3f(-1.0f, 0.0f, 0.0f);
            glVertex3f(x - s - offset, y - s, z + s);
            glVertex3f(x - s - offset, y + s, z + s);
            glVertex3f(x - s - offset, y + s, z - s);
            glVertex3f(x - s - offset, y - s, z - s);
            break;
        case 2:
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(x - s, y + s + offset, z - s);
            glVertex3f(x + s, y + s + offset, z - s);
            glVertex3f(x + s, y + s + offset, z + s);
            glVertex3f(x - s, y + s + offset, z + s);
            break;
        case 3:
            glNormal3f(0.0f, -1.0f, 0.0f);
            glVertex3f(x - s, y - s - offset, z + s);
            glVertex3f(x + s, y - s - offset, z + s);
            glVertex3f(x + s, y - s - offset, z - s);
            glVertex3f(x - s, y - s - offset, z - s);
            break;
        case 4:
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(x - s, y - s, z + s + offset);
            glVertex3f(x - s, y + s, z + s + offset);
            glVertex3f(x + s, y + s, z + s + offset);
            glVertex3f(x + s, y - s, z + s + offset);
            break;
        case 5:
            glNormal3f(0.0f, 0.0f, -1.0f);
            glVertex3f(x + s, y - s, z - s - offset);
            glVertex3f(x + s, y + s, z - s - offset);
            glVertex3f(x - s, y + s, z - s - offset);
            glVertex3f(x - s, y - s, z - s - offset);
            break;
    }
    glEnd();
}

void Renderer::drawCube(float x, float y, float z, float size) {
    float s = size / 2.0f;
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(x - s, y - s, z - s); glVertex3f(x + s, y - s, z - s);
    glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y - s, z + s);
    glVertex3f(x + s, y - s, z + s); glVertex3f(x - s, y - s, z + s);
    glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y - s, z - s);
    glVertex3f(x - s, y + s, z - s); glVertex3f(x + s, y + s, z - s);
    glVertex3f(x + s, y + s, z - s); glVertex3f(x + s, y + s, z + s);
    glVertex3f(x + s, y + s, z + s); glVertex3f(x - s, y + s, z + s);
    glVertex3f(x - s, y + s, z + s); glVertex3f(x - s, y + s, z - s);
    glVertex3f(x - s, y - s, z - s); glVertex3f(x - s, y + s, z - s);
    glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y + s, z - s);
    glVertex3f(x + s, y - s, z + s); glVertex3f(x + s, y + s, z + s);
    glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y + s, z + s);
    glEnd();
}

void Renderer::drawCubeTranslucent(float x, float y, float z, float size, float alpha) {
    float s = size / 2.0f;
    glColor4f(0.1f, 0.1f, 0.1f, alpha);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(x - s, y - s, z - s); glVertex3f(x + s, y - s, z - s);
    glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y - s, z + s);
    glVertex3f(x + s, y - s, z + s); glVertex3f(x - s, y - s, z + s);
    glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y - s, z - s);
    glVertex3f(x - s, y + s, z - s); glVertex3f(x + s, y + s, z - s);
    glVertex3f(x + s, y + s, z - s); glVertex3f(x + s, y + s, z + s);
    glVertex3f(x + s, y + s, z + s); glVertex3f(x - s, y + s, z + s);
    glVertex3f(x - s, y + s, z + s); glVertex3f(x - s, y + s, z - s);
    glVertex3f(x - s, y - s, z - s); glVertex3f(x - s, y + s, z - s);
    glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y + s, z - s);
    glVertex3f(x + s, y - s, z + s); glVertex3f(x + s, y + s, z + s);
    glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y + s, z + s);
    glEnd();
}

Mat4x4 Renderer::getViewRotation4D() const {
    // Combine XY and ZW rotations for 4D viewing
    Mat4x4 r1 = rotate4D(PLANE_XY, cameraAngleY * 0.5f);
    Mat4x4 r2 = rotate4D(PLANE_ZW, viewAngleW_);
    return matMul(r2, r1);
}

Mat4x4 Renderer::getAnimationRotation(const AnimationState& anim) const {
    if (!anim.isAnimating || anim.plane < 0) return Mat4x4::identity();
    return rotate4D(anim.plane, anim.currentAngle);
}

void Renderer::drawEdge(const Vec4& a, const Vec4& b, const Mat4x4& viewRot, float wDist) {
    Vec4 pa = project4Dto3D(matMul(viewRot, a), wDist);
    Vec4 pb = project4Dto3D(matMul(viewRot, b), wDist);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.4f, 0.4f, 0.5f, 0.5f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex3f(pa.x, pa.y, pa.z);
    glVertex3f(pb.x, pb.y, pb.z);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void Renderer::drawVertex(const Vec4& pos, const Vertex4D& v, const Mat4x4& viewRot, float wDist) {
    Vec4 p = project4Dto3D(matMul(viewRot, pos), wDist);
    float size = 0.38f;  // Rubik-style cubie (chunkier, like inner cube)
    const float alpha = 0.35f;  // Translucent outer cube
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    drawFaceTranslucent(0, 0, 0, size, 0, v.colors[0], alpha);
    drawFaceTranslucent(0, 0, 0, size, 1, 8, alpha);
    drawFaceTranslucent(0, 0, 0, size, 2, v.colors[1], alpha);
    drawFaceTranslucent(0, 0, 0, size, 3, 8, alpha);
    drawFaceTranslucent(0, 0, 0, size, 4, v.colors[2], alpha);
    drawFaceTranslucent(0, 0, 0, size, 5, v.colors[3], alpha);
    drawCubeTranslucent(0, 0, 0, size, alpha);
    glPopMatrix();
    glDisable(GL_BLEND);
}

void Renderer::render(const TesseractPuzzle& puzzle, const RubikCube* innerCube, int windowWidth, int windowHeight,
                     const AnimationState& anim, const RubikAnimState& rubikAnim) {
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float fov = 45.0f * (float)(M_PI / 180.0);
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float f = 1.0f / tanf(fov / 2.0f);
    float frustum[16] = {
        f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (farPlane + nearPlane) / (nearPlane - farPlane), -1.0f,
        0.0f, 0.0f, (2.0f * farPlane * nearPlane) / (nearPlane - farPlane), 0.0f
    };
    glMultMatrixf(frustum);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float radX = cameraAngleX * (float)(M_PI / 180.0);
    float radY = cameraAngleY * (float)(M_PI / 180.0);
    float camX = cameraDistance * cosf(radX) * sinf(radY);
    float camY = cameraDistance * sinf(radX);
    float camZ = cameraDistance * cosf(radX) * cosf(radY);
    float forward[3] = {-camX, -camY, -camZ};
    float len = sqrtf(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);
    forward[0] /= len; forward[1] /= len; forward[2] /= len;
    float up[3] = {0.0f, 1.0f, 0.0f};
    float right[3] = {
        forward[1]*up[2] - forward[2]*up[1],
        forward[2]*up[0] - forward[0]*up[2],
        forward[0]*up[1] - forward[1]*up[0]
    };
    len = sqrtf(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    right[0] /= len; right[1] /= len; right[2] /= len;
    float up2[3] = {
        right[1]*forward[2] - right[2]*forward[1],
        right[2]*forward[0] - right[0]*forward[2],
        right[0]*forward[1] - right[1]*forward[0]
    };
    float view[16] = {
        right[0], up2[0], -forward[0], 0.0f,
        right[1], up2[1], -forward[1], 0.0f,
        right[2], up2[2], -forward[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(view);
    glTranslatef(-camX, -camY, -camZ);

    drawStars();

    Mat4x4 viewRot = getViewRotation4D();
    Mat4x4 animRot = getAnimationRotation(anim);

    // Inner cube drawn first (before blending/translucent outer) to avoid GL state conflicts
    if (innerCube) {
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
        glDisable(GL_BLEND);
        glMatrixMode(GL_MODELVIEW);
        float innerScale = 0.6f;
        float cubieSize = 0.95f * innerScale;
        float spacing = 1.0f * innerScale;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
                    Vec4 pos4(x * spacing, y * spacing, z * spacing, 0.0f);
                    Vec4 proj = project4Dto3D(matMul(viewRot, pos4), wDistance_);
                    if (std::isfinite(proj.x) && std::isfinite(proj.y) && std::isfinite(proj.z))
                        drawCubieRubik(proj.x, proj.y, proj.z, cubieSize, *innerCube, x, y, z, rubikAnim);
                }
            }
        }
        glPopAttrib();
    }

    Vec4 positions[16];
    for (int ix = 0; ix < 2; ix++)
        for (int iy = 0; iy < 2; iy++)
            for (int iz = 0; iz < 2; iz++)
                for (int iw = 0; iw < 2; iw++) {
                    int idx = vindex(ix, iy, iz, iw);
                    Vec4 p = outerPositions_[idx];
                    bool inSlice = anim.isAnimating && TesseractPuzzle::isVertexInSlice(idx, anim.plane, anim.layer);
                    p = inSlice ? matMul(animRot, p) : p;
                    p = applyRubikAnimToVertex(p, ix, iy, iz, iw, rubikAnim);
                    positions[idx] = p;
                }

    for (int i = 0; i < 32; i++) {
        int a = EDGES[i][0], b = EDGES[i][1];
        drawEdge(positions[a], positions[b], viewRot, wDistance_);
    }

    for (int i = 0; i < 16; i++) {
        const Vertex4D& vert = puzzle.getVertex(i/8, (i/4)%2, (i/2)%2, i%2);
        drawVertex(positions[i], vert, viewRot, wDistance_);
    }
}

void Renderer::handleMouseDrag(int deltaX, int deltaY) {
    cameraAngleY += deltaX * 0.5f;
    cameraAngleX += deltaY * 0.5f;
    cameraAngleX = std::max(-89.0f, std::min(89.0f, cameraAngleX));
}

void Renderer::handleMouseWheel(int delta) {
    cameraDistance += delta * 0.2f;
    cameraDistance = std::max(3.0f, std::min(15.0f, cameraDistance));
}

void Renderer::rotate4DView(float deltaAngle) {
    viewAngleW_ += deltaAngle;
}

void Renderer::resetCamera() {
    cameraAngleX = 30.0f;
    cameraAngleY = 45.0f;
    cameraDistance = 8.0f;
    viewAngleW_ = 15.0f;
}

void Renderer::commitOuterRubikRotation(int face, bool clockwise) {
    Mat4x4 rot = rubikFaceRotation(face, clockwise);
    for (int ix = 0; ix < 2; ix++)
        for (int iy = 0; iy < 2; iy++)
            for (int iz = 0; iz < 2; iz++)
                for (int iw = 0; iw < 2; iw++) {
                    bool inSlice = false;
                    switch (face) {
                        case 0: inSlice = (ix == 1); break;
                        case 1: inSlice = (ix == 0); break;
                        case 2: inSlice = (iy == 1); break;
                        case 3: inSlice = (iy == 0); break;
                        case 4: inSlice = (iz == 1); break;
                        case 5: inSlice = (iz == 0); break;
                        default: break;
                    }
                    if (inSlice) {
                        int idx = vindex(ix, iy, iz, iw);
                        outerPositions_[idx] = matMul(rot, outerPositions_[idx]);
                    }
                }
}

void Renderer::resetOuterPositions() {
    for (int ix = 0; ix < 2; ix++)
        for (int iy = 0; iy < 2; iy++)
            for (int iz = 0; iz < 2; iz++)
                for (int iw = 0; iw < 2; iw++) {
                    int idx = vindex(ix, iy, iz, iw);
                    getVertexPos(ix, iy, iz, iw, outerPositions_[idx]);
                }
}
