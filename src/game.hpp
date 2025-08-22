#pragma once

#include "input.hpp"

#include <cstdint>
#include <string>
#include <vector>

#define GAME_MODE_COUNT 4

struct GameMode {
    std::string mode_name;

    std::vector<GameDirection> pattern;
    int pattern_size;
};

enum InputAccuracy : uint8_t { NONE = 0, SUCCESS, FAIL };

struct GameState {
    int player_pos{};

    uint64_t score{};
    uint64_t highscore{};

    GameDirection curr_input{};

    InputAccuracy last_input_acc{};
    GameDirection last_input{};

    GameMode current_mode;
    bool run_game{};
};

auto InitGame(std::vector<GameMode> &gamemodes) -> GameState;
void Update(ControllerState *cs, const std::vector<GameMode> &gamemodes,
            GameState &gamestate);

void initGameModes(std::vector<GameMode> &gamemodes);
void startGame(GameState &gamestate);
void updateMenu(ControllerState *cs, const std::vector<GameMode> &gamemodes,
                GameState &gamestate);
void updateGame(ControllerState *, GameState &gamestate);

extern int selected_mode;
extern ControllerState prev_input;
extern uint64_t highscores[GAME_MODE_COUNT];
