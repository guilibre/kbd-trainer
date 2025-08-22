#include "render.hpp"

#include "game.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>

static const std::string fontAsset = "assets/scorefont.ttf";

int ViewWidth = INITIAL_VIEW_WIDTH;
int ViewHeight = INITIAL_VIEW_HEIGHT;

std::vector<SDL_Texture *> direction_textures;

uint64_t curr_score = -1;
uint64_t curr_highscore = -2;

TTF_Font *score_font;
SDL_Texture *score_texture;
SDL_Texture *highscore_texture;

std::array<SDL_Texture *, 3> acc_textures;
std::array<SDL_Texture *, GAME_MODE_COUNT> menu_textures;

SDL_FRect next_input_rect = {
    SIDE_PADDING, ((float)INITIAL_VIEW_HEIGHT / 2) - ((float)ICON_HEIGHT / 2),
    ICON_WIDTH, ICON_HEIGHT};

SDL_FRect score_rect = {ICON_WIDTH + (SIDE_PADDING * 2), VERT_PADDING,
                        SCORE_COUNTER_WIDTH, SCORE_COUNTER_HEIGHT};

SDL_FRect highscore_rect = {ICON_WIDTH + (SIDE_PADDING * 2),
                            SCORE_COUNTER_HEIGHT + VERT_PADDING,
                            SCORE_COUNTER_WIDTH, SCORE_COUNTER_HEIGHT};

SDL_FRect last_input_acc_rect = {
    ((float)(ICON_WIDTH + (SIDE_PADDING * 2)) / 2) -
        ((float)ACC_DISPLAY_WIDTH / 2),
    INITIAL_VIEW_HEIGHT - ACC_DISPLAY_HEIGHT - VERT_PADDING, ACC_DISPLAY_WIDTH,
    ACC_DISPLAY_HEIGHT};

SDL_FRect failed_input_rect = {((float)(ICON_WIDTH + SIDE_PADDING * 2) / 2) -
                                   ((float)ACC_DISPLAY_WIDTH / 2),
                               INITIAL_VIEW_HEIGHT - VERT_PADDING - 27, 24, 24};

SDL_FRect failed_acc_rect = {((float)(ICON_WIDTH + SIDE_PADDING * 2) / 2) -
                                 ((float)ACC_DISPLAY_WIDTH / 2) + 30,
                             INITIAL_VIEW_HEIGHT - ACC_DISPLAY_HEIGHT -
                                 VERT_PADDING,
                             ACC_DISPLAY_WIDTH - 30, ACC_DISPLAY_HEIGHT};

void InitTextures(SDL_Renderer *renderer) {
    static const auto directionAssets =
        std::array{"assets/n.bmp",  "assets/u.bmp",  "assets/uf.bmp",
                   "assets/f.bmp",  "assets/df.bmp", "assets/d.bmp",
                   "assets/db.bmp", "assets/b.bmp",  "assets/ub.bmp"};

    for (const auto &directionAsset : directionAssets) {
        SDL_Texture *curr = IMG_LoadTexture(renderer, directionAsset);

        if (curr == nullptr)
            throw std::runtime_error("Error loading asset(" +
                                     std::string(directionAsset) +
                                     "): " + std::string(SDL_GetError()));

        direction_textures.emplace_back(curr);
    }

    highscore_texture = nullptr;
    score_texture = nullptr;

    score_font = TTF_OpenFont(fontAsset.c_str(), 24);
    if (score_font == nullptr)
        throw std::runtime_error("Error loading font asset(" + fontAsset +
                                 "): " + std::string(SDL_GetError()));

    SDL_Surface *surface = nullptr;
    acc_textures[0] = nullptr;

    SDL_Color successColor = {51, 255, 51};
    const char *successText = "GREAT";
    surface = TTF_RenderText_Solid(score_font, successText, strlen(successText),
                                   successColor);
    acc_textures[SUCCESS] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    SDL_Color failColor = {255, 51, 51};
    const char *failText = "MISS";
    surface =
        TTF_RenderText_Solid(score_font, failText, strlen(failText), failColor);
    acc_textures[FAIL] = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
}

void updateScore(SDL_Renderer *renderer, const GameState &gamestate) {
    SDL_Color scoreColor = {255, 255, 255};
    if (gamestate.score == curr_score && gamestate.highscore == curr_highscore)
        return;

    // Update score and destroy old texture
    curr_score = gamestate.score;

    if (score_texture != nullptr) SDL_DestroyTexture(score_texture);

    std::string scoreText = std::to_string(gamestate.score);
    SDL_Surface *scoreSurface = TTF_RenderText_Solid(
        score_font, scoreText.c_str(), scoreText.length(), scoreColor);

    score_texture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_DestroySurface(scoreSurface);

    if (gamestate.highscore == curr_highscore) return;

    curr_highscore = gamestate.highscore;
    if (highscore_texture != nullptr) SDL_DestroyTexture(highscore_texture);

    scoreText = std::to_string(gamestate.highscore);
    scoreSurface = TTF_RenderText_Solid(score_font, scoreText.c_str(),
                                        scoreText.length(), scoreColor);

    highscore_texture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_DestroySurface(scoreSurface);
}

void InitMenuTextures(SDL_Renderer *renderer,
                      const std::vector<GameMode> &gamemodes) {
    SDL_Color textColor = {255, 255, 255};

    for (int i = 0; i < GAME_MODE_COUNT; i++) {
        SDL_Surface *surface =
            TTF_RenderText_Solid(score_font, gamemodes[i].mode_name.c_str(),
                                 gamemodes[i].mode_name.length(), textColor);

        menu_textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (menu_textures[i] == nullptr)
            throw std::runtime_error("Error initializing mode select view: " +
                                     std::string(SDL_GetError()));
    }
}

void DestroyMenuTextures() {
    for (auto &menu_texture : menu_textures)
        SDL_DestroyTexture(menu_texture);
}

void Render(SDL_Renderer *renderer, GameState &gamestate) {
    if (gamestate.run_game)
        renderGame(renderer, gamestate);
    else
        renderMenu(renderer);
}

void renderMenu(SDL_Renderer *renderer) {
    SDL_RenderClear(renderer);

    SDL_FRect destRect = {SIDE_PADDING,
                          ((float)INITIAL_VIEW_HEIGHT / 2) -
                              ((float)ICON_HEIGHT / 2),
                          INITIAL_VIEW_WIDTH - (SIDE_PADDING * 2), ICON_HEIGHT};
    SDL_RenderTexture(renderer, menu_textures[selected_mode], nullptr,
                      &destRect);

    SDL_RenderPresent(renderer);
}

void renderGame(SDL_Renderer *renderer, GameState &gamestate) {
    SDL_RenderClear(renderer);

    SDL_Texture *nextInputTexture =
        direction_textures[gamestate.current_mode
                               .pattern[gamestate.player_pos %
                                        gamestate.current_mode.pattern_size]];
    SDL_RenderTexture(renderer, nextInputTexture, nullptr, &next_input_rect);

    updateScore(renderer, gamestate);
    SDL_RenderTexture(renderer, score_texture, nullptr, &score_rect);
    SDL_RenderTexture(renderer, highscore_texture, nullptr, &highscore_rect);

    if (gamestate.last_input_acc == FAIL) {
        SDL_RenderTexture(renderer, direction_textures[gamestate.last_input],
                          nullptr, &failed_input_rect);
        SDL_RenderTexture(renderer, acc_textures[gamestate.last_input_acc],
                          nullptr, &failed_acc_rect);

        SDL_RenderPresent(renderer);

        SDL_DelayPrecise(500000000);
        SDL_DelayPrecise(500000000);
        SDL_DelayPrecise(200000000);

        return;
    }

    SDL_RenderPresent(renderer);
}
