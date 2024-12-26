#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "SDL_events.h"

namespace Input
{
    void UpdateFrame();
    void HandleEvent(SDL_Event event);

    bool IsKeyJustPressed(SDL_Scancode scancode);
    bool IsKeyJustReleased(SDL_Scancode scancode);
    bool IsKeyDown(SDL_Scancode scancode);
    float GetAxis(SDL_Scancode negative, SDL_Scancode positive);
}

#endif // INPUT_H