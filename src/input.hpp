#pragma once

#include <cstdint>

enum GameDirection : uint8_t {
    NEUTRAL = 0,
    UP,
    UP_FORWARD,
    FORWARD,
    DOWN_FORWARD,
    DOWN,
    DOWN_BACK,
    BACK,
    UP_BACK,
    UNKNOWN,
    DISCONNECTED
};

struct ControllerState {
    GameDirection direction;
    bool select_pressed;
    bool back_pressed;
};

auto InitController() -> bool;
auto PollController() -> ControllerState *;
void parseDirection();