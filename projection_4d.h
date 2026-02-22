// 4D to 3D Projection
// Perspective projection from 4D to 3D for rendering

#ifndef PROJECTION_4D_H
#define PROJECTION_4D_H

#include "math_4d.h"

// Project 4D point to 3D using perspective projection
// wDistance: distance of projection plane along W axis (larger = less perspective)
// Returns (x,y,z) in 3D; caller uses x,y,z for rendering
Vec4 project4Dto3D(const Vec4& p, float wDistance);

#endif // PROJECTION_4D_H
