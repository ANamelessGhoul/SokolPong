#pragma once
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "sokol_gfx.h"
#include "math_types.hpp"
#include "stb_truetype.h"

constexpr size_t MaxQuads = 1024;

#pragma pack(push, 1)
struct Vertex{
    Vector3 position;
    Color color;
    uint8_t textureIndex;
    uint8_t _padding[3];
    Vector2 uv;
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

typedef stbtt_bakedchar FontChar;

struct Font{
    FontChar chars[128];
};

enum class FontAlignment{
    Left,
    Center,
    Right,
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

    bool hasFont = false;
    FontChar fontChars[128];
public:

    void Initialize();
    void Shutdown();

    void LoadFont(const char* path, float fontSize);

    void BeginDrawing();
    void EndDrawing();

    void BeginCamera(Camera2D camera);
    void EndCamera();

    void BeginUI();
    void EndUI();

    void SetClearColor(Color color);

    void DrawRectangle(Vector2 position, Vector2 size, Color color, uint8_t texture = UINT8_MAX, Vector4 uv = {0, 0, 1, 1}, float depth = 0);
    void DrawText(Vector2 position, const char* text, Color color, FontAlignment horizontalAlignment = FontAlignment::Left);
    float MeasureText(const char* text);
};



#endif // RENDERER_HPP