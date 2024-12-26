// Based on glfw_glue.c included in https://github.com/floooh/sokol-samples


#include "sokol_gfx.h"
#include "sdl_glue.h"
#include "assert.h"
#include <stdio.h>
#include "sokol_log.h"

static int _sample_count;
static bool _no_depth_buffer;
static int _major_version;
static int _minor_version;
static SDL_Window* _window;
static SDL_GLContext _context;


#define _sdl_def(val, def) (((val) == 0) ? (def) : (val))

bool sdl_init(const sdl_desc_t* desc) {
    assert(desc);
    assert(desc->width > 0);
    assert(desc->height > 0);
    assert(desc->title);
    sdl_desc_t desc_def = *desc;
    desc_def.sample_count = _sdl_def(desc_def.sample_count, 1);
    desc_def.version_major = _sdl_def(desc_def.version_major, 4);
    desc_def.version_minor = _sdl_def(desc_def.version_minor, 1);
    _sample_count = desc_def.sample_count;
    _no_depth_buffer = desc_def.no_depth_buffer;
    _major_version = desc_def.version_major;
    _minor_version = desc_def.version_minor;


    /* First, initialize SDL's video subsystem. */
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        /* Failed, exit. */
        printf("Video initialization failed: %s\n", SDL_GetError( ) );
        return false;
    }

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, desc_def.version_major );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, desc_def.version_minor );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    if (desc_def.no_depth_buffer) {
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    }

    _window = SDL_CreateWindow( desc_def.title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, desc_def.width, desc_def.height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
    if( _window == NULL )
    {
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    //Create context
    _context = SDL_GL_CreateContext( _window );
    if( _context == NULL )
    {
        printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    return true;
}

SDL_Window* sdl_window(void) {
    return _window;
}

void sdl_terminate(void)
{
    //Destroy window	
	SDL_DestroyWindow( _window );
	_window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int sdl_width(void) {
    int width, height;
    SDL_GL_GetDrawableSize(_window, &width, &height);
    return width;
}

int sdl_height(void) {
    int width, height;
    SDL_GL_GetDrawableSize(_window, &width, &height);
    return height;
}

sg_environment sdl_environment(void) {
    return (sg_environment) {
        .defaults = {
            .color_format = SG_PIXELFORMAT_RGBA8,
            .depth_format = _no_depth_buffer ? SG_PIXELFORMAT_NONE : SG_PIXELFORMAT_DEPTH_STENCIL,
            .sample_count = _sample_count,
        },
    };
}

sg_swapchain sdl_swapchain(void) {
    int width, height;
    SDL_GL_GetDrawableSize(_window, &width, &height);
    return (sg_swapchain) {
        .width = width,
        .height = height,
        .sample_count = _sample_count,
        .color_format = SG_PIXELFORMAT_RGBA8,
        .depth_format = _no_depth_buffer ? SG_PIXELFORMAT_NONE : SG_PIXELFORMAT_DEPTH_STENCIL,
        .gl = {
            // we just assume here that the GL framebuffer is always 0
            .framebuffer = 0,
        }
    };
}

