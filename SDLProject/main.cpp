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
