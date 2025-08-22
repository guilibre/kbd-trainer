#pragma once

#include "game.hpp"

#include <SDL3/SDL.h>

void InitTextures(SDL_Renderer *renderer);

void InitMenuTextures(SDL_Renderer *renderer,
                      const std::vector<GameMode> &gamemodes);
void DestroyMenuTextures();

void Render(SDL_Renderer *, GameState &gamestate);
void renderMenu(SDL_Renderer *);
void renderGame(SDL_Renderer *, GameState &gamestate);
void playFailAnimation(SDL_Renderer *);

void updateScore(SDL_Renderer *renderer, const GameState &gamestate);

constexpr auto ICON_WIDTH = 70;
constexpr auto ICON_HEIGHT = 70;

constexpr auto SIDE_PADDING = 30;
constexpr auto VERT_PADDING = 15;

constexpr auto SCORE_COUNTER_WIDTH = 200;
constexpr auto SCORE_COUNTER_HEIGHT = 70;

constexpr auto ACC_DISPLAY_HEIGHT = 30;
constexpr auto ACC_DISPLAY_WIDTH = 100;

const auto INITIAL_VIEW_WIDTH =
    (ICON_WIDTH + SCORE_COUNTER_WIDTH + (SIDE_PADDING * 3));
const auto INITIAL_VIEW_HEIGHT = ((ICON_HEIGHT * 2) + (VERT_PADDING * 2));