// 4D Math Library
// Vec4, Mat4x4, and 4D plane rotations (XY, XZ, XW, YZ, YW, ZW)

#ifndef MATH_4D_H
#define MATH_4D_H

// 4D vector
struct Vec4 {
    float x, y, z, w;
    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
};

// 4x4 matrix (column-major for OpenGL compatibility)
struct Mat4x4 {
    float m[16];
    Mat4x4();
    static Mat4x4 identity();
};

// 4D rotation in plane (axis1, axis2) by angle in degrees
// Planes: XY=0, XZ=1, XW=2, YZ=3, YW=4, ZW=5
Mat4x4 rotate4D(int plane, float angleDeg);

// Matrix-vector multiply: out = m * v
Vec4 matMul(const Mat4x4& m, const Vec4& v);

// Matrix multiply: out = a * b
Mat4x4 matMul(const Mat4x4& a, const Mat4x4& b);

#endif // MATH_4D_H
