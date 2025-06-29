#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include "utils.h"

#include <random>

static std::mt19937 rng = {};

struct SnakeSegment {
    Point position = {};
    Point direction = {};
};

struct Game {
    static constexpr i32 Width = 32;
    static constexpr i32 Height = 18;

    static constexpr f64 TickLength = 0.15;

    static constexpr Color BackgroundColor = Black;
    static constexpr Color SnakeColor = White;
    static constexpr Color FoodColor = Red;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    int width = 0;
    int height = 0;
    float tile_width = 0;
    float tile_height = 0;

    Clock clock = {};
    f64 tick_timer = 0.0;

    Point food = {static_cast<i32>(rng() % Width), static_cast<i32>(rng() % Height)};
    bool updated = true;
    std::vector<SnakeSegment> snake = {
        {{Width / 2, Height / 2}, Up},
    };
};

SDL_AppResult SDL_AppInit(void** appstate, int, char**) {
    rng.seed(std::random_device{}());

    *appstate = new Game{};
    Game& game = **reinterpret_cast<Game**>(appstate);

    SDL_Init(NULL);

    game.window = SDL_CreateWindow("Snake", 1920, 1080, NULL);
    if (game.window == nullptr)
        return SDL_APP_FAILURE;

    SDL_GetWindowSize(game.window, &game.width, &game.height);
    game.tile_width = static_cast<f32>(game.width) / static_cast<f32>(Game::Width);
    game.tile_height = static_cast<f32>(game.height) / static_cast<f32>(Game::Height);

    game.renderer = SDL_CreateRenderer(game.window, "");
    if (game.renderer == nullptr)
        return SDL_APP_FAILURE;

    SDL_SetRenderDrawColor(game.renderer, Game::BackgroundColor.r, Game::BackgroundColor.g, Game::BackgroundColor.b, Game::BackgroundColor.a);
    SDL_RenderClear(game.renderer);

    SDL_FRect snake_rect = {
        .x = 0.5f * game.tile_width * Game::Width,
        .y = 0.5f * game.tile_width * Game::Height,
        .w = game.tile_width,
        .h = game.tile_width,
    };
    SDL_SetRenderDrawColor(game.renderer, Game::SnakeColor.r, Game::SnakeColor.g, Game::SnakeColor.b, Game::SnakeColor.a);
    SDL_RenderFillRect(game.renderer, &snake_rect);

    SDL_RenderPresent(game.renderer);

    game.clock.update();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    Game& game = *reinterpret_cast<Game*>(appstate);

    game.clock.update();
    game.tick_timer += game.clock.delta_sec();

    if (game.tick_timer > Game::TickLength) {
        game.tick_timer -= Game::TickLength;

        SDL_SetRenderDrawColor(game.renderer, Game::BackgroundColor.r, Game::BackgroundColor.g, Game::BackgroundColor.b, Game::BackgroundColor.a);
        SDL_RenderClear(game.renderer);

        Point next_position = game.snake[0].position + game.snake[0].direction;
        if (next_position.x < 0)
            next_position.x = Game::Width - 1;
        if (next_position.y < 0)
            next_position.y = Game::Height - 1;
        if (next_position.x >= Game::Width)
            next_position.x = 0;
        if (next_position.y >= Game::Height)
            next_position.y = 0;

        for (usize i = 1; i < game.snake.size(); ++i) {
            if (next_position == game.snake[i].position) {
                return SDL_APP_SUCCESS;
            }
        }

        if (next_position == game.food) {
            game.food = {static_cast<i32>(rng() % Game::Width), static_cast<i32>(rng() % Game::Height)};

            game.snake.emplace_back(game.snake.back());
            game.snake.back().position = game.snake.back().position - game.snake.back().direction;
        }

        SDL_FRect head_rect = {
            .x = next_position.x * game.tile_width,
            .y = next_position.y * game.tile_width,
            .w = game.tile_width,
            .h = game.tile_width,
        };
        SDL_SetRenderDrawColor(game.renderer, Game::SnakeColor.r, Game::SnakeColor.g, Game::SnakeColor.b, Game::SnakeColor.a);
        SDL_RenderFillRect(game.renderer, &head_rect);

        game.snake[0].position = next_position;

        for (usize i = game.snake.size() - 1; i >= 1; --i) {
            game.snake[i].position += game.snake[i].direction;
            if (game.snake[i].position.x < 0)
                game.snake[i].position.x = Game::Width - 1;
            if (game.snake[i].position.y < 0)
                game.snake[i].position.y = Game::Height - 1;
            if (game.snake[i].position.x >= Game::Width)
                game.snake[i].position.x = 0;
            if (game.snake[i].position.y >= Game::Height)
                game.snake[i].position.y = 0;

            game.snake[i].direction = game.snake[i - 1].direction;

            SDL_FRect segment_rect = {
                .x = game.snake[i].position.x * game.tile_width,
                .y = game.snake[i].position.y * game.tile_width,
                .w = game.tile_width,
                .h = game.tile_width,
            };
            SDL_SetRenderDrawColor(game.renderer, Game::SnakeColor.r, Game::SnakeColor.g, Game::SnakeColor.b, Game::SnakeColor.a);
            SDL_RenderFillRect(game.renderer, &segment_rect);
        }

        SDL_FRect food_rect = {
            .x = game.food.x * game.tile_width,
            .y = game.food.y * game.tile_width,
            .w = game.tile_width,
            .h = game.tile_width,
        };
        SDL_SetRenderDrawColor(game.renderer, Game::FoodColor.r, Game::FoodColor.g, Game::FoodColor.b, Game::FoodColor.a);
        SDL_RenderFillRect(game.renderer, &food_rect);

        SDL_RenderPresent(game.renderer);
        game.updated = true;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    Game& game = *reinterpret_cast<Game*>(appstate);

    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_KEY_DOWN:
        if (game.updated) {
            switch (event->key.key) {
            case SDLK_W:
                if (game.snake[0].direction != Down) {
                    game.snake[0].direction = Up;
                    game.updated = false;
                }
                break;
            case SDLK_S:
                if (game.snake[0].direction != Up) {
                    game.snake[0].direction = Down;
                    game.updated = false;
                }
                break;
            case SDLK_A:
                if (game.snake[0].direction != Right) {
                    game.snake[0].direction = Left;
                    game.updated = false;
                }
                break;
            case SDLK_D:
                if (game.snake[0].direction != Left) {
                    game.snake[0].direction = Right;
                    game.updated = false;
                }
                break;
            }
        }
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    Game& state = *reinterpret_cast<Game*>(appstate);

    if (result == SDL_APP_FAILURE) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL Error: %s", SDL_GetError());
    }

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();

    delete &state;
}
