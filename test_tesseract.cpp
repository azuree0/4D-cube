// Smoke tests for tesseract puzzle logic
// Run: build/Release/test_tesseract.exe

#include "tesseract_model.h"
#include "math_4d.h"
#include "projection_4d.h"
#include <iostream>
#include <cassert>

static int tests_run = 0;
static int tests_failed = 0;

#define TEST(name) do { ++tests_run; std::cout << "  " << name << " ... "; } while(0)
#define PASS() do { std::cout << "OK\n"; } while(0)
#define FAIL(msg) do { ++tests_failed; std::cout << "FAIL: " << (msg) << "\n"; } while(0)

void test_solved_state() {
    TEST("Solved state");
    TesseractPuzzle p;
    if (p.isSolved()) PASS();
    else FAIL("fresh puzzle should be solved");
}

void test_reset() {
    TEST("Reset returns to solved");
    TesseractPuzzle p;
    p.scramble(5);
    p.reset();
    if (p.isSolved()) PASS();
    else FAIL("reset should restore solved state");
}

void test_inverse_move() {
    TEST("Inverse move restores state");
    TesseractPuzzle p;
    p.rotateSlice(PLANE_XY, 0, true);   // CW
    p.rotateSlice(PLANE_XY, 0, false);  // CCW
    if (p.isSolved()) PASS();
    else FAIL("XY0 + XY0' should be identity");
}

void test_four_moves_identity() {
    TEST("Four same moves = identity");
    TesseractPuzzle p;
    for (int i = 0; i < 4; i++) p.rotateSlice(PLANE_XY, 0, true);
    if (p.isSolved()) PASS();
    else FAIL("4× XY0 CW should be identity");
}

void test_projection_finite() {
    TEST("Projection produces finite values");
    Vec4 p(1.0f, 1.0f, 1.0f, 1.0f);
    Vec4 out = project4Dto3D(p, 4.0f);
    bool ok = (out.x == out.x && out.y == out.y && out.z == out.z);
    if (ok) PASS();
    else FAIL("projection should not produce NaN");
}

void test_math_rotate() {
    TEST("4D rotation matrix");
    Mat4x4 r = rotate4D(PLANE_XY, 90.0f);
    Vec4 v(1.0f, 0.0f, 0.0f, 0.0f);
    Vec4 out = matMul(r, v);
    // 90° CW in XY: (1,0,0,0) -> (0,-1,0,0)
    bool ok = (out.x > -0.01f && out.x < 0.01f && out.y > -1.01f && out.y < -0.99f);
    if (ok) PASS();
    else FAIL("90° XY rotation of (1,0,0,0) should give ~(0,-1,0,0)");
}

int main() {
    std::cout << "Tesseract smoke tests\n";
    test_solved_state();
    test_reset();
    test_inverse_move();
    test_four_moves_identity();
    test_projection_finite();
    test_math_rotate();
    std::cout << "\n" << tests_run << " tests, " << tests_failed << " failed\n";
    return tests_failed ? 1 : 0;
}
