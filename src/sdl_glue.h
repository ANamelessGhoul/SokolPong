// Based on glfw_glue.h included in https://github.com/floooh/sokol-samples


#pragma once

#ifndef SDL_GLUE_H
#define SDL_GLUE_H

#include "sokol_gfx.h"
#include "SDL.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct sdl_desc_t {
    int width;
    int height;
    int sample_count;
    bool no_depth_buffer;
    const char* title;
    int version_major;
    int version_minor;
} sdl_desc_t;

bool sdl_init(const sdl_desc_t* desc);
SDL_Window* sdl_window(void);
int sdl_width(void);
int sdl_height(void);
sg_environment sdl_environment(void);
sg_swapchain sdl_swapchain(void);
void sdl_terminate(void);

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // SDL_GLUE_H