#pragma once

namespace Engine {

struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
};

struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vector4 {
    float x, y, z, w;
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct Matrix4x4 {
    float m[16];
    Matrix4x4() {
        for (int i = 0; i < 16; ++i) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
};

struct Guid {
    uint64_t low;
    uint64_t high;
    Guid() : low(0), high(0) {}
    Guid(uint64_t l, uint64_t h) : low(l), high(h) {}
};

} // namespace Engine
