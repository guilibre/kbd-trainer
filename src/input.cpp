#include "input.hpp"

#include <SDL3/SDL.h>
#include <cstdio>
#include <iostream>

SDL_Gamepad *gamepad = nullptr;
ControllerState controller_state;
short dpad_state = 0;

auto InitController() -> bool {
    SDL_SetGamepadEventsEnabled(false);

    int numOfPads = 0;
    SDL_JoystickID *padIds = SDL_GetGamepads(&numOfPads);

    if (numOfPads == 0) return false;

    for (int i = 0; i < numOfPads; i++) {
        if (SDL_IsGamepad(padIds[i])) {
            gamepad = SDL_OpenGamepad(padIds[i]);
            if (gamepad == nullptr) {
                std::cout << "Error initializing gamepad: " << SDL_GetError();
                return false;
            }

            return true;
        }
    }

    return false;
}

auto PollController() -> ControllerState * {
    if (gamepad != nullptr) {
        SDL_UpdateGamepads();
        dpad_state = 1 * static_cast<int>(SDL_GetGamepadButton(
                             gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP));
        dpad_state |= 2 * static_cast<int>(SDL_GetGamepadButton(
                              gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN));
        dpad_state |= 4 * static_cast<int>(SDL_GetGamepadButton(
                              gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT));
        dpad_state |= 8 * static_cast<int>(SDL_GetGamepadButton(
                              gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT));

        controller_state.select_pressed =
            SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
        controller_state.back_pressed =
            SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_EAST) ||
            SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_START);
    } else {
        SDL_PumpEvents();
        const bool *keys = SDL_GetKeyboardState(nullptr);

        dpad_state =
            1 * static_cast<int>(keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]);
        dpad_state |= 2 * static_cast<int>(keys[SDL_SCANCODE_S] ||
                                           keys[SDL_SCANCODE_DOWN]);
        dpad_state |= 4 * static_cast<int>(keys[SDL_SCANCODE_A] ||
                                           keys[SDL_SCANCODE_LEFT]);
        dpad_state |= 8 * static_cast<int>(keys[SDL_SCANCODE_D] ||
                                           keys[SDL_SCANCODE_RIGHT]);

        if ((dpad_state & 3) == 3) dpad_state &= 12;

        if ((dpad_state & 12) == 12) dpad_state &= 3;

        controller_state.select_pressed = keys[SDL_SCANCODE_SPACE] ||
                                          keys[SDL_SCANCODE_RETURN] ||
                                          keys[SDL_SCANCODE_RETURN2];
        controller_state.back_pressed = keys[SDL_SCANCODE_ESCAPE];
    }

    parseDirection();

    return &controller_state;
}

void parseDirection() {
    switch (dpad_state & 0xF) {
    case 0x0:
        controller_state.direction = NEUTRAL;
        break;
    case 0x1:
        controller_state.direction = UP;
        break;
    case 0x9:
        controller_state.direction = UP_FORWARD;
        break;
    case 0x5:
        controller_state.direction = UP_BACK;
        break;
    case 0x4:
        controller_state.direction = BACK;
        break;
    case 0x8:
        controller_state.direction = FORWARD;
        break;
    case 0x2:
        controller_state.direction = DOWN;
        break;
    case 0xA:
        controller_state.direction = DOWN_FORWARD;
        break;
    case 0x6:
        controller_state.direction = DOWN_BACK;
        break;
    default:
        controller_state.direction = UNKNOWN;
    }
}