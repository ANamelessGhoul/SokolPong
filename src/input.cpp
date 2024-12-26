#include "input.hpp"

namespace Input
{

    static struct {
        bool keys[SDL_NUM_SCANCODES] = {0};
        bool keys_previous[SDL_NUM_SCANCODES] = {0};
    } state;

    void UpdateFrame()
    {
        for (size_t i = 0; i < SDL_NUM_SCANCODES; i++)
        {
            state.keys_previous[i] = state.keys[i];
        }
    }

    void HandleEvent(SDL_Event event)
    {
        if (event.type == SDL_KEYDOWN)
        {
            state.keys[event.key.keysym.scancode] = true;
        }
        else if (event.type == SDL_KEYUP)
        {
            state.keys[event.key.keysym.scancode] = false;
        }
    }

    bool IsKeyJustPressed(SDL_Scancode scancode)
    {
        return !state.keys_previous[scancode] && state.keys[scancode];
    }

    bool IsKeyJustReleased(SDL_Scancode scancode)
    {
        return state.keys_previous[scancode] && !state.keys[scancode];
    }

    bool IsKeyDown(SDL_Scancode scancode)
    {
        return state.keys[scancode];
    }

    float GetAxis(SDL_Scancode negative, SDL_Scancode positive)
    {
        return IsKeyDown(negative) * -1.f + IsKeyDown(positive) * 1.f;
    };

} // namespace Input
