#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "sokol_gfx.h"
#include "math_types.hpp"

constexpr size_t MaxQuads = 1024;

#pragma pack(push, 1)
struct Vertex{
    Vector3 position;
    Color color;
};

struct Quad{
    Vertex vertices[4];
};
#pragma pack(pop)

struct DrawFrame
{
    Quad quads[MaxQuads];
    size_t count;
};

struct Camera2D{
    Vector2 position;
    Vector2 zoom;
};

class Renderer
{
private:
    DrawFrame draw_frame{0};

	sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;

    Matrix projection;
    Matrix view;
public:

    void Initialize();
    void Shutdown();

    void BeginDrawing();
    void EndDrawing();

    void BeginCamera(Camera2D camera);
    void EndCamera();

    void SetClearColor(Color color);

    void DrawRectangle(Vector2 position, Vector2 size, Color color, float depth = 0);
};



#endif // RENDERER_HPP