// 3D OpenGL Renderer Implementation
// Handles all OpenGL rendering, camera control, and animation

#include "renderer.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Constructor - initialize camera position
Renderer::Renderer() {
    cameraAngleX = 30.0f;
    cameraAngleY = 45.0f;
    cameraDistance = 8.0f;
}

// Draw starfield background
void Renderer::drawStars() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f); // White stars
    
    // Generate stars in a sphere around the origin
    // Using a simple pseudo-random distribution
    std::srand(42); // Fixed seed for consistent star positions
    for (int i = 0; i < 150; i++) {
        // Generate random point on sphere
        float theta = (float)(std::rand() % 628) / 100.0f; // 0 to 2*PI
        float phi = (float)(std::rand() % 314) / 100.0f;   // 0 to PI
        float radius = 50.0f; // Far distance
        
        float x = radius * sin(phi) * cos(theta);
        float y = radius * sin(phi) * sin(theta);
        float z = radius * cos(phi);
        
        glVertex3f(x, y, z);
    }
    
    // Add some brighter stars
    glPointSize(3.0f);
    glColor3f(1.0f, 1.0f, 0.9f); // Slightly yellow for brighter stars
    for (int i = 0; i < 15; i++) {
        float theta = (float)(std::rand() % 628) / 100.0f;
        float phi = (float)(std::rand() % 314) / 100.0f;
        float radius = 50.0f;
        
        float x = radius * sin(phi) * cos(theta);
        float y = radius * sin(phi) * sin(theta);
        float z = radius * cos(phi);
        
        glVertex3f(x, y, z);
    }
    
    glEnd();
    
    // Re-enable lighting and depth test
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

// Initialize OpenGL settings and lighting
void Renderer::initialize() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);  // Show all faces
    
    // Setup lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Set up light with strong specular for reflections
    GLfloat lightPos[] = {5.0f, 5.0f, 5.0f, 1.0f};
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat lightSpecular[] = {1.5f, 1.5f, 1.5f, 1.0f}; // Increased specular intensity
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    // Enable smooth shading for better reflections
    glShadeModel(GL_SMOOTH);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
}

// Set OpenGL color based on face color enum
void Renderer::setColor(int faceColor) {
    switch (faceColor) {
        case WHITE:
            glColor3f(1.0f, 1.0f, 1.0f);
            break;
        case YELLOW:
            glColor3f(1.0f, 1.0f, 0.0f);
            break;
        case RED:
            glColor3f(1.0f, 0.0f, 0.0f);
            break;
        case ORANGE:
            glColor3f(1.0f, 0.5f, 0.0f);
            break;
        case GREEN:
            glColor3f(0.0f, 1.0f, 0.0f);
            break;
        case BLUE:
            glColor3f(0.0f, 0.0f, 1.0f);
            break;
        default:
            glColor3f(0.2f, 0.2f, 0.2f);
            break;
    }
}

// Draw a single colored face of a cubie
void Renderer::drawFace(float x, float y, float z, float size, int faceIndex, int color) {
    float s = size / 2.0f;
    float offset = 0.01f; // Small offset to make faces slightly protrude and avoid z-fighting
    
    // Set material properties for highly reflective surface
    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Maximum specular reflection
    GLfloat matShininess[] = {128.0f}; // Maximum shininess for mirror-like reflection
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

// Draw cube edges (black lines)
void Renderer::drawCube(float x, float y, float z, float size) {
    float s = size / 2.0f;
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    
    // 12 edges of the cube
    // Bottom face
    glVertex3f(x - s, y - s, z - s); glVertex3f(x + s, y - s, z - s);
    glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y - s, z + s);
    glVertex3f(x + s, y - s, z + s); glVertex3f(x - s, y - s, z + s);
    glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y - s, z - s);
    
    // Top face
    glVertex3f(x - s, y + s, z - s); glVertex3f(x + s, y + s, z - s);
    glVertex3f(x + s, y + s, z - s); glVertex3f(x + s, y + s, z + s);
    glVertex3f(x + s, y + s, z + s); glVertex3f(x - s, y + s, z + s);
    glVertex3f(x - s, y + s, z + s); glVertex3f(x - s, y + s, z - s);
    
    // Vertical edges
    glVertex3f(x - s, y - s, z - s); glVertex3f(x - s, y + s, z - s);
    glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y + s, z - s);
    glVertex3f(x + s, y - s, z + s); glVertex3f(x + s, y + s, z + s);
    glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y + s, z + s);
    
    glEnd();
}

// Main render function - sets up view and draws entire cube
void Renderer::render(const RubikCube& cube, int windowWidth, int windowHeight, const AnimationState& anim) {
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float fov = 45.0f * M_PI / 180.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float f = 1.0f / tan(fov / 2.0f);
    float frustum[16] = {
        f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (farPlane + nearPlane) / (nearPlane - farPlane), -1.0f,
        0.0f, 0.0f, (2.0f * farPlane * nearPlane) / (nearPlane - farPlane), 0.0f
    };
    glMultMatrixf(frustum);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Camera positioning
    float radX = cameraAngleX * M_PI / 180.0f;
    float radY = cameraAngleY * M_PI / 180.0f;
    float camX = cameraDistance * cos(radX) * sin(radY);
    float camY = cameraDistance * sin(radX);
    float camZ = cameraDistance * cos(radX) * cos(radY);
    
    // Manual lookAt matrix
    float forward[3] = {-camX, -camY, -camZ};
    float length = sqrt(forward[0]*forward[0] + forward[1]*forward[1] + forward[2]*forward[2]);
    forward[0] /= length; forward[1] /= length; forward[2] /= length;
    
    float up[3] = {0.0f, 1.0f, 0.0f};
    float right[3] = {
        forward[1] * up[2] - forward[2] * up[1],
        forward[2] * up[0] - forward[0] * up[2],
        forward[0] * up[1] - forward[1] * up[0]
    };
    length = sqrt(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    right[0] /= length; right[1] /= length; right[2] /= length;
    
    float up2[3] = {
        right[1] * forward[2] - right[2] * forward[1],
        right[2] * forward[0] - right[0] * forward[2],
        right[0] * forward[1] - right[1] * forward[0]
    };
    
    float view[16] = {
        right[0], up2[0], -forward[0], 0.0f,
        right[1], up2[1], -forward[1], 0.0f,
        right[2], up2[2], -forward[2], 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(view);
    glTranslatef(-camX, -camY, -camZ);
    
    // Draw background stars
    drawStars();
    
    // Draw all 27 cubies (3x3x3 grid)
    const auto& faces = cube.getFaces();
    float cubieSize = 0.95f;
    float spacing = 1.0f;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                float posX = x * spacing;
                float posY = y * spacing;
                float posZ = z * spacing;
                
                drawCubie(posX, posY, posZ, cubieSize, cube, x, y, z, anim);
            }
        }
    }
}

// Draw a single cubie with appropriate colors and rotation animation
void Renderer::drawCubie(float x, float y, float z, float size, const RubikCube& cube, int cubieX, int cubieY, int cubieZ, const AnimationState& anim) {
    const auto& faces = cube.getFaces();
    bool isRotating = false;
    float rotationAngle = 0.0f;
    int rotationAxis = 0; // 0=X, 1=Y, 2=Z
    float faceCenterX = 0.0f, faceCenterY = 0.0f, faceCenterZ = 0.0f;
    
    // Check if this cubie is part of the rotating face
    if (anim.isAnimating) {
        switch (anim.face) {
            case RIGHT: // Rotate around X axis, x = 1
                if (cubieX == 1) {
                    isRotating = true;
                    rotationAxis = 0;
                    rotationAngle = anim.currentAngle;
                    faceCenterX = 1.0f; faceCenterY = 0.0f; faceCenterZ = 0.0f;
                }
                break;
            case LEFT: // Rotate around X axis, x = -1
                if (cubieX == -1) {
                    isRotating = true;
                    rotationAxis = 0;
                    rotationAngle = -anim.currentAngle; // Opposite direction
                    faceCenterX = -1.0f; faceCenterY = 0.0f; faceCenterZ = 0.0f;
                }
                break;
            case UP: // Rotate around Y axis, y = 1
                if (cubieY == 1) {
                    isRotating = true;
                    rotationAxis = 1;
                    rotationAngle = anim.currentAngle;
                    faceCenterX = 0.0f; faceCenterY = 1.0f; faceCenterZ = 0.0f;
                }
                break;
            case DOWN: // Rotate around Y axis, y = -1
                if (cubieY == -1) {
                    isRotating = true;
                    rotationAxis = 1;
                    rotationAngle = -anim.currentAngle;
                    faceCenterX = 0.0f; faceCenterY = -1.0f; faceCenterZ = 0.0f;
                }
                break;
            case FRONT: // Rotate around Z axis, z = 1
                if (cubieZ == 1) {
                    isRotating = true;
                    rotationAxis = 2;
                    rotationAngle = anim.currentAngle;
                    faceCenterX = 0.0f; faceCenterY = 0.0f; faceCenterZ = 1.0f;
                }
                break;
            case BACK: // Rotate around Z axis, z = -1
                if (cubieZ == -1) {
                    isRotating = true;
                    rotationAxis = 2;
                    rotationAngle = -anim.currentAngle;
                    faceCenterX = 0.0f; faceCenterY = 0.0f; faceCenterZ = -1.0f;
                }
                break;
        }
    }
    
    // Apply rotation if needed - rotate around face center, not cubie center
    if (isRotating) {
        glPushMatrix();
        // Translate to face center
        glTranslatef(faceCenterX, faceCenterY, faceCenterZ);
        
        // Rotate around the appropriate axis
        switch (rotationAxis) {
            case 0: // X axis
                glRotatef(rotationAngle, 1.0f, 0.0f, 0.0f);
                break;
            case 1: // Y axis
                glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
                break;
            case 2: // Z axis
                glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
                break;
        }
        
        // Translate back from face center, then to cubie position
        glTranslatef(-faceCenterX, -faceCenterY, -faceCenterZ);
        glTranslatef(x, y, z);
    } else {
        // No rotation, just translate to cubie position
        glPushMatrix();
        glTranslatef(x, y, z);
    }
    
    // Draw all 6 faces of the cubie with colors from cube state
    // Right face (+X) - Red
    int row = 1 - cubieY;
    int col = 1 - cubieZ;
    drawFace(0, 0, 0, size, 0, faces[RIGHT][row][col]);
    
    // Left face (-X) - Orange
    row = 1 - cubieY;
    col = cubieZ + 1;
    drawFace(0, 0, 0, size, 1, faces[LEFT][row][col]);
    
    // Up face (+Y) - White
    row = cubieZ + 1;
    col = cubieX + 1;
    drawFace(0, 0, 0, size, 2, faces[UP][row][col]);
    
    // Down face (-Y) - Yellow
    row = 1 - cubieZ;
    col = cubieX + 1;
    drawFace(0, 0, 0, size, 3, faces[DOWN][row][col]);
    
    // Front face (+Z) - Green
    row = 1 - cubieY;
    col = cubieX + 1;
    drawFace(0, 0, 0, size, 4, faces[FRONT][row][col]);
    
    // Back face (-Z) - Blue
    row = 1 - cubieY;
    col = 1 - cubieX;
    drawFace(0, 0, 0, size, 5, faces[BACK][row][col]);
    
    // Draw black edges around cubie
    drawCube(0, 0, 0, size);
    
    glPopMatrix();
}

// Handle mouse drag for camera rotation
void Renderer::handleMouseDrag(int deltaX, int deltaY) {
    cameraAngleY += deltaX * 0.5f;
    cameraAngleX += deltaY * 0.5f;
    
    // Clamp vertical rotation
    cameraAngleX = std::max(-89.0f, std::min(89.0f, cameraAngleX));
}

// Handle mouse wheel for zoom in/out
void Renderer::handleMouseWheel(int delta) {
    cameraDistance += delta * 0.2f;
    cameraDistance = std::max(3.0f, std::min(15.0f, cameraDistance));
}

// Reset camera to default position
void Renderer::resetCamera() {
    cameraAngleX = 30.0f;
    cameraAngleY = 45.0f;
    cameraDistance = 8.0f;
}

