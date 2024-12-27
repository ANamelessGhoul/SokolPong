//------------------------------------------------------------------------------
//  clear-sdl.c
//  Simple draw loop, clear default framebuffer.
//------------------------------------------------------------------------------

#include "SDL_main.h"
#include "SDL_timer.h"
#include <cassert>
#include "sdl_glue.h"

#include "math_types.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "game_state.hpp"

#include <ctime>

#ifndef RESOURCES_PATH 
#define RESOURCES_PATH "./resources/"
#endif

struct {
    Uint64 startTicks;
    Uint64 lastFrameTicks;
} gAppState;

GameState gGameState;

double UpdateDeltaTime()
{
    Uint64 now = SDL_GetPerformanceCounter();
    double delta = (now - gAppState.lastFrameTicks) / static_cast<double>(SDL_GetPerformanceFrequency());
    gAppState.lastFrameTicks = now;
    return delta;
}

double GetTime()
{
    return (SDL_GetPerformanceCounter() - gAppState.startTicks) / static_cast<double>(SDL_GetPerformanceFrequency());
}

void FitGameArea(Camera2D& camera)
{
    int width, height;
    SDL_GL_GetDrawableSize(sdl_window(), &width, &height);
    float horizontalRatio = width / gGameParams.gameSize.X;
    float verticalRatio = height / gGameParams.gameSize.Y;
    if (horizontalRatio < verticalRatio)
    {
        // Fit width
        camera.zoom = {horizontalRatio, horizontalRatio};
    }
    else
    {
        // Fit height
        camera.zoom = {verticalRatio, verticalRatio};
    }
}

void ResetGameElements()
{
    gGameState.paddlePositionLeft = {-gGameParams.gameSize.X * 0.45f - gGameParams.paddleSize.X * 0.5f, gGameParams.paddleSize.Y * -0.5f};
    gGameState.paddlePositionRight = {gGameParams.gameSize.X * 0.45f - gGameParams.paddleSize.X * 0.5f, gGameParams.paddleSize.Y * -0.5f};


    gGameState.ballPosition = {gGameParams.ballSize * -0.5f};
    float randFloat = (rand() / static_cast<float>(RAND_MAX));
    if (randFloat < 0.5f)
    {
        float angle = (randFloat - 0.25f) * HMM_PI32;
        gGameState.ballVelocity = HMM_RotateV2({gGameParams.ballSpeed, 0}, angle);
    }
    else
    {
        float angle = (randFloat - 0.75f) * HMM_PI32;
        gGameState.ballVelocity = HMM_RotateV2({-gGameParams.ballSpeed, 0}, angle);
    }
}

bool CheckCollisionRectangles(Vector2 position1, Vector2 size1, Vector2 position2, Vector2 size2)
{
    return ((position1.X < (position2.X + size2.X) && (position1.X + size1.X) > position2.X) &&
        (position1.Y < (position2.Y + size2.Y) && (position1.Y + size1.Y) > position2.Y));
}

void DrawDigit(int digit, Renderer& renderer, Vector2 position, Vector2 size, Color color)
{
    assert(digit < 10);

    constexpr int FontWidth = 3;
    constexpr int FontHeight = 5;
    constexpr int FontDigitLength = FontWidth * FontHeight;
    constexpr bool Digits[] = {
        // 0
        1, 1, 1,
        1, 0, 1,
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
        
        // 1
        0, 1, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,

        // 2
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,

        // 3
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,

        // 4
        1, 0, 1,
        1, 0, 1,
        1, 1, 1,
        0, 0, 1,
        0, 0, 1,
        
        // 5
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
        0, 0, 1,
        1, 1, 1,

        // 6
        1, 1, 1,
        1, 0, 0,
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,

        // 7
        1, 1, 1,
        0, 0, 1,
        0, 1, 0,
        0, 1, 0,
        0, 1, 0,

        // 8
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,

        // 9
        1, 1, 1,
        1, 0, 1,
        1, 1, 1,
        0, 0, 1,
        0, 0, 1,
    };

    float pixelWidth = size.X / FontWidth;
    float pixelHeight = size.Y / FontHeight;

    int offset = FontDigitLength * digit;
    for (int y = 0; y < FontHeight; y++)
    {
        for (int x = 0; x < FontWidth; x++)
        {
            if(!Digits[offset + y * FontWidth + x])
            {
                continue;
            }
            
            renderer.DrawRectangle(position + Vector2{pixelWidth * x, size.Y - pixelHeight * (y + 1)}, {pixelWidth, pixelHeight}, color);
        }
    }
    

}

int SDL_main( int argc, char* args[] )
{
    // setup sdl
    sdl_desc_t sdl_desc = { .width = 640, .height = 360, .title = "SDL Sokol" };
    bool success = sdl_init(&sdl_desc);
    assert(success);

    srand(time(NULL));

    Renderer renderer{};
    renderer.Initialize();
    // TODO: move clear color to application layer? Because we will want to have multiple renderers later
    renderer.SetClearColor(ColorFromHex(0x181818FF));
   
    gAppState.startTicks = SDL_GetPerformanceCounter();
    gAppState.lastFrameTicks = gAppState.startTicks;

    gGameState.scoreRight = 9;
    
    renderer.LoadFont(RESOURCES_PATH "Kenney Pixel.ttf", 64.f);

    ResetGameElements();

    bool quit = false;
    SDL_Event e;
    while( !quit )
    {
        double deltaTime = UpdateDeltaTime();

        Input::UpdateFrame();

        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }

            Input::HandleEvent(e);
        }

        if(Input::IsKeyJustPressed(SDL_SCANCODE_F11))
        {
            Uint32 windowFlags = SDL_GetWindowFlags(sdl_window());
            if (windowFlags & SDL_WINDOW_FULLSCREEN_DESKTOP > 0)
            {
                SDL_SetWindowFullscreen(sdl_window(), 0);
            }
            else
            {
                SDL_SetWindowFullscreen(sdl_window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        }

        if (gGameState.state == PONG_WAITING)
        {
            if(Input::IsKeyJustPressed(SDL_SCANCODE_SPACE))
            {
                gGameState.state = PONG_PLAYING;
            }
        }
        else if (gGameState.state == PONG_PLAYING)
        {
            float inputLeft = Input::GetAxis(SDL_SCANCODE_S, SDL_SCANCODE_W);
            float inputRight = Input::GetAxis(SDL_SCANCODE_DOWN, SDL_SCANCODE_UP);

            gGameState.paddlePositionLeft += Vector2Up * (gGameParams.paddleSpeed * inputLeft * deltaTime);
            gGameState.paddlePositionRight += Vector2Up * (gGameParams.paddleSpeed * inputRight * deltaTime);

            gGameState.ballPosition += gGameState.ballVelocity * deltaTime;

            // Check top wall
            if (gGameState.ballPosition.Y + gGameParams.ballSize.Y > (gGameParams.gameSize.Y * 0.5f) - gGameParams.wallThickness)
            {
                gGameState.ballPosition.Y = ((gGameParams.gameSize.Y * 0.5f) - gGameParams.wallThickness) - gGameParams.ballSize.Y;
                gGameState.ballVelocity.Y *= -1;
            }
            // Check bottom wall
            else if (gGameState.ballPosition.Y < (-gGameParams.gameSize.Y * 0.5f) + gGameParams.wallThickness)
            {
                gGameState.ballPosition.Y = ((-gGameParams.gameSize.Y * 0.5f) + gGameParams.wallThickness);
                gGameState.ballVelocity.Y *= -1;
            }

            // Check Paddles
            if (CheckCollisionRectangles(gGameState.paddlePositionLeft, gGameParams.paddleSize, gGameState.ballPosition, gGameParams.ballSize))
            {
                gGameState.ballPosition.X = gGameState.paddlePositionLeft.X + gGameParams.paddleSize.X;
                gGameState.ballVelocity.X *= -1;
            }
            else if (CheckCollisionRectangles(gGameState.paddlePositionRight, gGameParams.paddleSize, gGameState.ballPosition, gGameParams.ballSize))
            {
                gGameState.ballPosition.X = gGameState.paddlePositionRight.X - gGameParams.ballSize.X;
                gGameState.ballVelocity.X *= -1;
            }

            if (gGameState.ballPosition.X > gGameParams.gameSize.X * 0.5f)
            {
                gGameState.scoreLeft += 1;

                if (gGameState.scoreLeft >= 10)
                {
                    gGameState.state = PONG_END;
                }
                else
                {
                    gGameState.state = PONG_WAITING;
                    ResetGameElements();
                }
            }
            else if (gGameState.ballPosition.X < -gGameParams.gameSize.X * 0.5f - gGameParams.ballSize.X)
            {
                gGameState.scoreRight += 1;

                if (gGameState.scoreRight >= 10)
                {
                    gGameState.state = PONG_END;
                }
                else
                {
                    gGameState.state = PONG_WAITING;
                    ResetGameElements();
                }
            }
        }
        else if (gGameState.state == PONG_END)
        {
            if(Input::IsKeyJustPressed(SDL_SCANCODE_SPACE))
            {
                gGameState.state = PONG_PLAYING;
                ResetGameElements();

                gGameState.scoreLeft = 0;
                gGameState.scoreRight = 0;
            }
        }


        Camera2D camera{.zoom = {1.f, 1.f}};
        FitGameArea(camera);

        renderer.BeginDrawing();

        renderer.BeginCamera(camera);
            // Draw paddles and ball
            renderer.DrawRectangle(gGameState.paddlePositionLeft, gGameParams.paddleSize, gGameParams.paddleColor);
            renderer.DrawRectangle(gGameState.paddlePositionRight, gGameParams.paddleSize, gGameParams.paddleColor);
            renderer.DrawRectangle(gGameState.ballPosition, gGameParams.ballSize, gGameParams.ballColor);

            // Draw Top and Bottom walls
            Vector2 gameAreaSize = gGameParams.gameSize;
            renderer.DrawRectangle({-gameAreaSize.X * 0.5f, gameAreaSize.Y * 0.5f}, {gameAreaSize.X, -gGameParams.wallThickness}, gGameParams.wallColor);
            renderer.DrawRectangle({-gameAreaSize.X * 0.5f, -gameAreaSize.Y * 0.5f}, {gameAreaSize.X, gGameParams.wallThickness}, gGameParams.wallColor);
            
            // Draw line
            float lineHeight = (gameAreaSize.Y - gGameParams.lineGap * (gGameParams.lineSegments - 1)) / gGameParams.lineSegments;
            for (int i = 0; i < gGameParams.lineSegments; i++)
            {
                renderer.DrawRectangle({-gGameParams.lineThickness * 0.5f, -gGameParams.gameHeight * 0.5f + (lineHeight + gGameParams.lineGap) * i}, {gGameParams.lineThickness, lineHeight}, gGameParams.lineColor);
            }
            
            DrawDigit(HMM_MIN(gGameState.scoreLeft, 9) , renderer, {2 * -gGameParams.scoreSize.X, (gGameParams.gameSize.Y * 0.5f) - (gGameParams.scoreSize.Y * 1.5f)}, gGameParams.scoreSize, gGameParams.scoreColor);
            DrawDigit(HMM_MIN(gGameState.scoreRight, 9), renderer, {     gGameParams.scoreSize.X, (gGameParams.gameSize.Y * 0.5f) - (gGameParams.scoreSize.Y * 1.5f)}, gGameParams.scoreSize, gGameParams.scoreColor);

            if(gGameState.state == PONG_END)
            {
                Color color = gGameParams.paddleColor;
                color.A = 0.25f + (sin(GetTime() * 5) + 1.f) * 0.25f;
                if (gGameState.scoreLeft >= 10)
                {
                    renderer.DrawRectangle({-gGameParams.gameSize.X * 0.5f, -gGameParams.gameSize.Y * 0.5f}, {gGameParams.gameSize.X * 0.1f, gGameParams.gameSize.Y}, color);
                }
                else if (gGameState.scoreRight >= 10)
                {
                    renderer.DrawRectangle({gGameParams.gameSize.X * 0.4f, -gGameParams.gameSize.Y * 0.5f}, {gGameParams.gameSize.X * 0.1f, gGameParams.gameSize.Y}, color);
                }
            }

        renderer.EndCamera();

        renderer.BeginUI();
            if (gGameState.state == PONG_WAITING)
            {
                renderer.DrawText({sdl_width() * 0.5f, sdl_height() * 0.7f}, "Press SPACE to begin", gGameParams.lineColor, FontAlignment::Center);
            }
            else if (gGameState.state == PONG_END)
            {
                renderer.DrawText({sdl_width() * 0.5f, sdl_height() * 0.5f}, "GAME OVER!", gGameParams.lineColor, FontAlignment::Center);
                renderer.DrawText({sdl_width() * 0.5f, sdl_height() * 0.5f + 48.f}, "Press SPACE to restart", gGameParams.lineColor, FontAlignment::Center);
            }
        renderer.EndUI();

        renderer.EndDrawing();
    }

    // shutdown sokol_gfx and sdl
    renderer.Shutdown();
    sdl_terminate();
    return 0;
}
