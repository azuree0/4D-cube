// 4D Math Library Implementation

#include "math_4d.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

Mat4x4::Mat4x4() {
    for (int i = 0; i < 16; i++) m[i] = 0.0f;
}

Mat4x4 Mat4x4::identity() {
    Mat4x4 r;
    r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
    return r;
}

// Build 4D rotation matrix in plane (i,j) by angle (radians)
// Column-major: m[col*4+row]
static void setRotation2D(Mat4x4& out, int i, int j, float c, float s) {
    out = Mat4x4::identity();
    out.m[i * 4 + i] = c;
    out.m[i * 4 + j] = -s;
    out.m[j * 4 + i] = s;
    out.m[j * 4 + j] = c;
}

Mat4x4 rotate4D(int plane, float angleDeg) {
    float rad = angleDeg * (float)(M_PI / 180.0);
    float c = std::cos(rad);
    float s = std::sin(rad);
    Mat4x4 r;
    switch (plane) {
        case 0: setRotation2D(r, 0, 1, c, s); break; // XY
        case 1: setRotation2D(r, 0, 2, c, s); break; // XZ
        case 2: setRotation2D(r, 0, 3, c, s); break; // XW
        case 3: setRotation2D(r, 1, 2, c, s); break; // YZ
        case 4: setRotation2D(r, 1, 3, c, s); break; // YW
        case 5: setRotation2D(r, 2, 3, c, s); break; // ZW
        default: r = Mat4x4::identity(); break;
    }
    return r;
}

Vec4 matMul(const Mat4x4& m, const Vec4& v) {
    return Vec4(
        m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12] * v.w,
        m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13] * v.w,
        m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14] * v.w,
        m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15] * v.w
    );
}

Mat4x4 matMul(const Mat4x4& a, const Mat4x4& b) {
    Mat4x4 r;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            r.m[col * 4 + row] = 0.0f;
            for (int k = 0; k < 4; k++)
                r.m[col * 4 + row] += a.m[k * 4 + row] * b.m[col * 4 + k];
        }
    }
    return r;
}
