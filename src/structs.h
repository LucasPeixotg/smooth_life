#include <SDL2/SDL.h>

struct app {
    SDL_Renderer* renderer;
    SDL_Window* window;

    int running;

    int mouse_down;
    int* mouse_x;
    int* mouse_y;
};