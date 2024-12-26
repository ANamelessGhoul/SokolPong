#include "renderer.hpp"
#include "sdl_glue.h"
#include "gen/quad.glsl.h"
#include <cassert>
#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol_gfx.h"
#include "sokol_log.h"

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
		.shader = sg_make_shader(quad_shader_desc(sg_query_backend())),
		.layout = {
			.attrs = {
				[ATTR_quad_position0] = { .format = sg_vertex_format::SG_VERTEXFORMAT_FLOAT3 },
				[ATTR_quad_color0] = { .format = sg_vertex_format::SG_VERTEXFORMAT_FLOAT4 },
			},
		},
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
		.index_type = sg_index_type::SG_INDEXTYPE_UINT16,
        // .cull_mode = SG_CULLMODE_BACK,
	};
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

void Renderer::SetClearColor(Color color)
{
    pass_action.colors->clear_value = {color.R, color.G, color.B, color.A};
}

void Renderer::DrawRectangle(Vector2 position, Vector2 size, Color color, float depth/* = 0*/)
{
    assert(draw_frame.count != MaxQuads && "Ran out of space for more quads");

    Matrix mvp = projection * view;

    Vector3 top_left     = HMM_MulM4V4(mvp, {position.X, position.Y, depth, 1.f}).XYZ;
    Vector3 bottom_left  = HMM_MulM4V4(mvp, {position.X, position.Y + size.Y, depth, 1.f}).XYZ;
    Vector3 bottom_right = HMM_MulM4V4(mvp, {position.X + size.X, position.Y + size.Y, depth, 1.f}).XYZ;
    Vector3 top_right    = HMM_MulM4V4(mvp, {position.X + size.X, position.Y, depth, 1.f}).XYZ;

    draw_frame.quads[draw_frame.count++] = {{
        {top_left    , color},
        {bottom_left , color},
        {bottom_right, color},
        {top_right   , color},
    }};
}
