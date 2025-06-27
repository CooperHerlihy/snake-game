#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

struct AppState {
    SDL_Window* window;
    SDL_Renderer* renderer;
};

SDL_AppResult SDL_AppInit(void** appstate, int, char**) {
    *appstate = new AppState;
    AppState& state = **reinterpret_cast<AppState**>(appstate);

    const auto init_success = SDL_Init(NULL);
    if (!init_success)
        return SDL_APP_FAILURE;

    state.window = SDL_CreateWindow("Snake", 1920, 1080, NULL);
    if (state.window == nullptr)
        return SDL_APP_FAILURE;

    state.renderer = SDL_CreateRenderer(state.window, "");
    if (state.renderer == nullptr)
        return SDL_APP_FAILURE;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    AppState& state = *reinterpret_cast<AppState*>(appstate);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState& state = *reinterpret_cast<AppState*>(appstate);

    switch(event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState& state = *reinterpret_cast<AppState*>(appstate);

    if (result == SDL_APP_FAILURE) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL Error: %s", SDL_GetError());
    }

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();

    delete &state;
}

