#pragma once

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "math_types.hpp"

enum PongState{
    PONG_WAITING,
    PONG_PLAYING,
    PONG_END
};

struct GameState{
    PongState state;

    Vector2 paddlePositionLeft;
    Vector2 paddlePositionRight;
    Vector2 ballPosition;
    Vector2 ballVelocity;
    int scoreLeft;
    int scoreRight;
};

extern GameState gGameState;

constexpr struct {
    Vector2 paddleSize = {10, 80};
    float paddleSpeed = 50.f;
    Color paddleColor = ColorFromHex(0xAAFFAAFF);

    Vector2 ballSize = {10, 10};
    float ballSpeed = 100.f;
    Color ballColor = ColorFromHex(0xAAAAFFFF);

    float wallThickness = 10.f;
    Color wallColor = ColorFromHex(0xAAFFAAFF);

    int gameWidth = 640;
    int gameHeight = 360;
    Vector2 gameSize = {static_cast<float>(gameWidth), static_cast<float>(gameHeight)};

    int lineSegments = 10;
    float lineThickness = 5.f;
    float lineGap = 20.f;
    Color lineColor = ColorFromHex(0xAAFFAA7F);

    Vector2 scoreSize = {24, 40};
    Color scoreColor = ColorFromHex(0xAAFFAA7F);
} gGameParams;

#endif // GAME_STATE_HPP