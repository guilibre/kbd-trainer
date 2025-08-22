#include "SDL3/SDL_error.h"
#include "game.hpp"
#include "input.hpp"
#include "render.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>

auto main() -> int {
    bool isRunning = true;
    bool showGameView = false;

    constexpr auto FRAME_DELAY = 16666666;
    Uint64 frameStart = 0;
    Uint64 frameTime = 0;
    Uint64 prevFrame = 0;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    auto exit_with_error = 0;
    try {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
            throw std::runtime_error("Error initializing SDL: " +
                                     std::string(SDL_GetError()));

        if (!TTF_Init())
            throw std::runtime_error("Error initializing SDL_ttf.\n");

        if (InitController())
            std::cout << "Controller found!\n";
        else
            std::cout
                << "No compatible controller detected. Using keyboard inputs "
                   "(WASD)\n";

        SDL_CreateWindowAndRenderer("KBD Trainer", INITIAL_VIEW_WIDTH,
                                    INITIAL_VIEW_HEIGHT, 0, &window, &renderer);

        if (window == nullptr)
            throw std::runtime_error("Error creating window: " +
                                     std::string(SDL_GetError()));

        if (renderer == nullptr)
            throw std::runtime_error("Error creating renderer: " +
                                     std::string(SDL_GetError()));

        std::vector<GameMode> gamemodes;
        GameState gamestate = InitGame(gamemodes);

        InitTextures(renderer);
        InitMenuTextures(renderer, gamemodes);

        std::cout
            << "Menu loaded. Let's go!\nPress LEFT/RIGHT to navigate\nA to "
               "confirm selection | B/Start to return to menu\n";

        SDL_Event ev;
        while (isRunning) {
            prevFrame = frameStart;
            frameStart = SDL_GetTicksNS();
            while (SDL_PollEvent(&ev))
                if (ev.type == SDL_EVENT_QUIT) isRunning = false;

            Update(PollController(), gamemodes, gamestate);
            Render(renderer, gamestate);

            frameTime = SDL_GetTicksNS() - frameStart;

            if (FRAME_DELAY > frameTime)
                SDL_DelayPrecise(FRAME_DELAY - frameTime);
        }
    } catch (const std::runtime_error &ex) {
        std::cerr << ex.what();
        exit_with_error = 1;
    }

    if (renderer != nullptr) SDL_DestroyRenderer(renderer);
    if (window != nullptr) SDL_DestroyWindow(window);
    SDL_Quit();

    return exit_with_error;
}