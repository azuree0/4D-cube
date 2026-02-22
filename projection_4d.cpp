// 4D to 3D Projection Implementation

#include "projection_4d.h"
#include <cmath>

// Perspective projection: project (x,y,z,w) onto 3D hyperplane at w = wDistance
// Formula: scale = wDistance / (wDistance + w), then (x,y,z) *= scale
Vec4 project4Dto3D(const Vec4& p, float wDistance) {
    float denom = wDistance + p.w;
    if (std::fabs(denom) < 1e-6f) denom = 1e-6f;
    float scale = wDistance / denom;
    return Vec4(p.x * scale, p.y * scale, p.z * scale, 0.0f);
}
