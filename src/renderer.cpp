#include "renderer.hpp"
#include "sdl_glue.h"
#include "gen/quad_uv.glsl.h"
#include <cassert>

#include "logging.h"

#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol_gfx.h"
#include "sokol_log.h"


#ifdef _DEBUG
    #define STB_IMAGE_WRITE_IMPLEMENTATION
    #include "stb_image_write.h"
#endif

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

void Renderer::Initialize()
{
    sg_desc desc = {
        .logger = {.func = slog_func},
        .environment = sdl_environment(),
    };
    sg_setup(&desc);
    assert(sg_isvalid());

    // default pass action, clear to black
    pass_action = {
        .colors = {{ .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f } } }
    };


    sg_buffer_desc vertex_buffer_desc = {
        .size = sizeof(Quad) * MaxQuads,
        .usage = sg_usage::SG_USAGE_DYNAMIC,
    };
    
	// make the vertex buffer
	bind.vertex_buffers[0] = sg_make_buffer(&vertex_buffer_desc);

    
	{ // make & fill the index buffer
        constexpr size_t index_buffer_count = MaxQuads * 6;
        uint16_t indices[index_buffer_count];
        for (size_t i = 0; i < index_buffer_count; i+=6) {
            // vertex offset pattern to draw a quad
            // { 0, 1, 2,  0, 2, 3 }
            indices[i + 0] = static_cast<uint16_t>((i/6)*4 + 0);
            indices[i + 1] = static_cast<uint16_t>((i/6)*4 + 1);
            indices[i + 2] = static_cast<uint16_t>((i/6)*4 + 2);
            indices[i + 3] = static_cast<uint16_t>((i/6)*4 + 0);
            indices[i + 4] = static_cast<uint16_t>((i/6)*4 + 2);
            indices[i + 5] = static_cast<uint16_t>((i/6)*4 + 3);
        }

        sg_buffer_desc index_buffer_desc = {
            .type = sg_buffer_type::SG_BUFFERTYPE_INDEXBUFFER,
            .data = { .ptr = indices, .size = sizeof(indices) },
        };
        bind.index_buffer = sg_make_buffer(index_buffer_desc);
    }

    // setup pipeline
	sg_pipeline_desc pipeline_desc = {
		.shader = sg_make_shader(quad_uv_shader_desc(sg_query_backend())),
		.layout = {
			.attrs = {
				[ATTR_quad_uv_position] = { .format = sg_vertex_format::SG_VERTEXFORMAT_FLOAT3 },
				[ATTR_quad_uv_color0] = { .format = sg_vertex_format::SG_VERTEXFORMAT_FLOAT4 },
				[ATTR_quad_uv_bytes0] = { .format = sg_vertex_format::SG_VERTEXFORMAT_BYTE4N },
				[ATTR_quad_uv_texcoord0] = { .format = sg_vertex_format::SG_VERTEXFORMAT_FLOAT2 },
			},
		},
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
		.index_type = sg_index_type::SG_INDEXTYPE_UINT16,
        // .cull_mode = SG_CULLMODE_BACK,
	};

    sg_sampler_desc sampler_desc = {0};
	bind.samplers[SMP_texture0_smp] = sg_make_sampler(sampler_desc);


	sg_blend_state blend_state = {
		.enabled = true,
		.src_factor_rgb = sg_blend_factor::SG_BLENDFACTOR_SRC_ALPHA,
		.dst_factor_rgb = sg_blend_factor::SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.op_rgb = sg_blend_op::SG_BLENDOP_ADD,
		.src_factor_alpha = sg_blend_factor::SG_BLENDFACTOR_ONE,
		.dst_factor_alpha = sg_blend_factor::SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
		.op_alpha = sg_blend_op::SG_BLENDOP_ADD,
	};
	pipeline_desc.colors[0] = { .blend = blend_state };
	pip = sg_make_pipeline(pipeline_desc);
}

void Renderer::Shutdown()
{
    sg_shutdown();
}

void Renderer::LoadFont(const char *path, float fontSize)
{
    static unsigned char temp_bitmap[1024*1024];
    
    void* fontFile = SDL_LoadFile(path, NULL);
    if (fontFile == NULL)
    {
        LOG(LOG_ERROR, "Could not open font file");
        SDL_ClearError();
        return; 
    }

    int result = stbtt_BakeFontBitmap((unsigned char*)fontFile, 0, fontSize, temp_bitmap, 1024, 1024, 32, 96, fontChars);
    SDL_free(fontFile);
    assert(result > 0 && "Could not fit font into temporary bitmap!");

#ifdef _DEBUG
    stbi_write_png("font.png", 1024, 1024, 1, temp_bitmap, 0);
#endif

    sg_image_desc font_image_desc {
        .type = SG_IMAGETYPE_2D,
        .width = 1024,
        .height = 1024,
        .pixel_format = SG_PIXELFORMAT_R8,
        .data = { .subimage = {{{.ptr = temp_bitmap, .size = 1024 * 1024}}}}
    };

    sg_image image = sg_make_image(font_image_desc);
    if (image.id == SG_INVALID_ID)
    {
        // TODO: Soft error
        assert("Could not load image data");
        return;
    }

    bind.images[IMG__texture0] = image;

    hasFont = true;

}

void Renderer::BeginDrawing()
{
    draw_frame.count = 0;
    projection = HMM_Orthographic_RH_NO(-sdl_width() / 2.f, sdl_width() / 2.0f, -sdl_height() / 2.f, sdl_height() / 2.0f, -100.f, 100.f);
    view = HMM_M4D(1.f);
}

void Renderer::EndDrawing()
{
    sg_update_buffer(
        bind.vertex_buffers[0],
        { .ptr = draw_frame.quads, .size = sizeof(Quad) * MaxQuads }
    );
    sg_pass pass = { .action = pass_action, .swapchain = sdl_swapchain() };
    sg_begin_pass(&pass);
    sg_apply_pipeline(pip);
    sg_apply_bindings(bind);
    sg_draw(0, 6*draw_frame.count, 1);
    sg_end_pass();
    sg_commit();

    SDL_GL_SwapWindow(sdl_window());
}

void Renderer::BeginCamera(Camera2D camera)
{
    Matrix cameraMat = HMM_Scale({camera.zoom.X, camera.zoom.Y, 0}) * HMM_Translate({-camera.position.X, -camera.position.Y, 0});
    view = cameraMat;    
}

void Renderer::EndCamera()
{
    view = HMM_M4D(1.0f);
}

void Renderer::BeginUI()
{
    projection = HMM_Orthographic_RH_NO(0, sdl_width(), sdl_height(), 0, -100.f, 100.f);
    view = HMM_M4D(1.0f);
}

void Renderer::EndUI()
{
    projection = HMM_Orthographic_RH_NO(-sdl_width() / 2.f, sdl_width() / 2.0f, -sdl_height() / 2.f, sdl_height() / 2.0f, -100.f, 100.f);
    view = HMM_M4D(1.0f);
}

void Renderer::SetClearColor(Color color)
{
    pass_action.colors->clear_value = {color.R, color.G, color.B, color.A};
}

void Renderer::DrawRectangle(Vector2 position, Vector2 size, Color color, uint8_t texture /* = UINT8_MAX */, Vector4 uv /* = {0, 0, 1, 1}  */, float depth /* = 0 */)
{
    assert(draw_frame.count != MaxQuads && "Ran out of space for more quads");

    Matrix mvp = projection * view;

    Vector3 top_left     = HMM_MulM4V4(mvp, {position.X, position.Y, depth, 1.f}).XYZ;
    Vector3 bottom_left  = HMM_MulM4V4(mvp, {position.X, position.Y + size.Y, depth, 1.f}).XYZ;
    Vector3 bottom_right = HMM_MulM4V4(mvp, {position.X + size.X, position.Y + size.Y, depth, 1.f}).XYZ;
    Vector3 top_right    = HMM_MulM4V4(mvp, {position.X + size.X, position.Y, depth, 1.f}).XYZ;

    draw_frame.quads[draw_frame.count++] = {{
        {top_left    , color, texture, {}, {uv[0], uv[1]}},
        {bottom_left , color, texture, {}, {uv[0], uv[3]}},
        {bottom_right, color, texture, {}, {uv[2], uv[3]}},
        {top_right   , color, texture, {}, {uv[2], uv[1]}},
    }};
}

#ifdef DrawText
#undef DrawText
#endif
void Renderer::DrawText(Vector2 position, const char *text, Color color, FontAlignment horizontalAlignment)
{
    assert(hasFont && "You can't draw text before loading a font!");

    // Make sure the quads are pixel aligned
    position = {roundf(position.X), roundf(position.Y)};

    float textWidth = MeasureText(text);

    float advance = 0.f;

    for (int i = 0; text[i] != '\0'; i++)
    {
        char next = text[i];
        FontChar fontChar =  fontChars[next - 32];

        
        Vector4 uv;
        uv[0] = fontChar.x0 / 1024.f;
        uv[1] = fontChar.y0 / 1024.f;
        uv[2] = fontChar.x1 / 1024.f;
        uv[3] = fontChar.y1 / 1024.f;

        Vector2 size = {fontChar.x1 - fontChar.x0, fontChar.y1 - fontChar.y0};

        Vector2 currentPosition = position;
        currentPosition += Vector2{advance + fontChar.xoff, fontChar.yoff};

        switch (horizontalAlignment)
        {
        case FontAlignment::Left:
            break;
        case FontAlignment::Center:
            currentPosition.X -= textWidth * 0.5f;
            break;
        case FontAlignment::Right:
            currentPosition.X -= textWidth;
            break;
        default:
            assert(true && "Invalid text alignment value");
            break;
        }

        advance += fontChar.xadvance;

        DrawRectangle(currentPosition, size, color, 0, uv);
    }
    
}

float Renderer::MeasureText(const char *text)
{
    assert(hasFont && "You can't measure text before loading a font!");

    float advance = 0.f;

    for (int i = 0; text[i] != '\0'; i++)
    {
        char next = text[i];
        FontChar fontChar =  fontChars[next - 32];
        advance += fontChar.xadvance;
    }

    return advance;
}
