#include "game.hpp"

#include "input.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

int selected_mode;
ControllerState prev_input;
uint64_t highscores[GAME_MODE_COUNT];

auto InitGame(std::vector<GameMode> &gamemodes) -> GameState {
    initGameModes(gamemodes);

    return GameState{.current_mode = gamemodes[0]};
}

void Update(ControllerState *cs, const std::vector<GameMode> &gamemodes,
            GameState &gamestate) {
    if (gamestate.run_game)
        updateGame(cs, gamestate);
    else
        updateMenu(cs, gamemodes, gamestate);
}

void updateMenu(ControllerState *cs, const std::vector<GameMode> &gamemodes,
                GameState &gamestate) {
    if (cs->back_pressed == prev_input.back_pressed &&
        cs->select_pressed == prev_input.select_pressed &&
        cs->direction == prev_input.direction)
        return;

    prev_input = *cs;

    // Start game
    if (cs->select_pressed) {
        startGame(gamestate);
        return;
    }

    if (cs->direction == FORWARD) {
        if (selected_mode == GAME_MODE_COUNT - 1)
            selected_mode = 0;
        else
            selected_mode += 1;
    } else if (cs->direction == BACK) {
        if (selected_mode == 0)
            selected_mode = GAME_MODE_COUNT - 1;
        else
            selected_mode -= 1;
    }

    gamestate.current_mode = gamemodes[selected_mode];
}

void updateGame(ControllerState *cs, GameState &gamestate) {
    gamestate.curr_input = cs->direction;
    if (gamestate.last_input_acc == FAIL) {
        // Reset after failure
        gamestate.player_pos = 0;
        gamestate.score = 0;
        gamestate.last_input_acc = NONE;
        prev_input = *cs;
        return;
    }

    if (cs->back_pressed) {
        highscores[selected_mode] = gamestate.highscore;
        gamestate.run_game = false;
        return;
    }

    // No update if input has not changed
    if (cs->direction == prev_input.direction) return;

    if (cs->direction ==
        gamestate.current_mode.pattern[gamestate.player_pos %
                                       gamestate.current_mode.pattern_size]) {
        // Correct input
        gamestate.score += 50;
        gamestate.highscore = std::max(gamestate.score, gamestate.highscore);

        if (gamestate.player_pos == gamestate.current_mode.pattern_size - 1)
            gamestate.player_pos = 0;
        else
            gamestate.player_pos += 1;

        prev_input.direction = cs->direction;

        gamestate.last_input = cs->direction;
        gamestate.last_input_acc = SUCCESS;
    } else {
        // Incorrect input
        prev_input.direction = cs->direction;

        if (gamestate.player_pos != 0) {
            gamestate.last_input = cs->direction;
            gamestate.last_input_acc = FAIL;
        }
    }
}

void startGame(GameState &gamestate) {
    std::cout << "Mode: " << selected_mode
              << " | score: " << highscores[selected_mode] << "\n";
    gamestate.player_pos = 0;
    gamestate.score = 0;
    gamestate.highscore = highscores[selected_mode];

    gamestate.last_input = NEUTRAL;
    gamestate.last_input_acc = NONE;
    gamestate.run_game = true;
}

void initGameModes(std::vector<GameMode> &gamemodes) {
    gamemodes.emplace_back(
        GameMode{"P1 KBD", {BACK, NEUTRAL, BACK, DOWN_BACK}, 4});

    gamemodes.emplace_back(
        GameMode{"P2 KBD", {FORWARD, NEUTRAL, FORWARD, DOWN_FORWARD}, 4});

    gamemodes.emplace_back(GameMode{
        "P1 WD", {FORWARD, NEUTRAL, DOWN, DOWN_FORWARD, FORWARD, NEUTRAL}, 6});

    gamemodes.emplace_back(
        GameMode{"P2 WD", {BACK, NEUTRAL, DOWN, DOWN_BACK, BACK, NEUTRAL}, 6});

    std::cout << "Gamemodes Initialized.\n";
}