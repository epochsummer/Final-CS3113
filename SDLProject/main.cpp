/**
* Author: Seha Kim
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Scene.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    Scene game;
    game.Initialize();

    SDL_Event event;

    while (game.IsRunning()) {
        while (SDL_PollEvent(&event)) {
            game.ProcessInput(event);
        }

        float ticks = (float)SDL_GetTicks() / 1000.0f;
        static float lastTicks = ticks;
        float deltaTime = ticks - lastTicks;
        lastTicks = ticks;

        game.Update(deltaTime);
        game.Render();
    }

    return 0;
}
