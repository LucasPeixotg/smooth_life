#include "SDL2/SDL.h"

#include <stdio.h>
#include <stdlib.h>
#include<time.h>

#include "consts.h"
#include "structs.h"

#include "automaton.h"

struct app app;
struct automaton* automaton;
unsigned total_frames = 0;

void initSDL(void) {
    app.mouse_down = FALSE;
    app.mouse_x = malloc(sizeof(int));
    app.mouse_y = malloc(sizeof(int));
    

    *app.mouse_x = 0;
    *app.mouse_y = 0;
    
    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    app.window = SDL_CreateWindow("game of life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SWIDTH, SHEIGHT, windowFlags);

    if (!app.window) {
        printf("Failed to open %d x %d window: %s\n", SWIDTH, SHEIGHT, SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

    if (!app.renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
}

void handle_input(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_MOUSEBUTTONDOWN:
                app.mouse_down = TRUE;
                break;
            case SDL_MOUSEBUTTONUP:
                app.mouse_down = FALSE;
                break;
            default:
                break;
        }
    }

    
}

void render(void) {
    SDL_SetRenderDrawColor(app.renderer, 16, 16, 16, 255);
    SDL_RenderClear(app.renderer);

    int cell_width = SWIDTH / GWIDTH;
    int cell_height = SHEIGHT / GHEIGHT;

    for(int i = 0; i < GWIDTH; i++) {
        for(int j = 0; j < GHEIGHT; j++) {            
            SDL_SetRenderDrawColor(app.renderer, 
                automaton->matrix[i][j]*122, 
                automaton->matrix[i][j]*242, 
                automaton->matrix[i][j]*255, 255 );
            SDL_Rect rect;
            rect.x = cell_width * i;
            rect.y = cell_height * j;
            rect.w = cell_width;
            rect.h = cell_height;
            SDL_RenderFillRect(app.renderer, &rect);
        }
    }
    

    SDL_RenderPresent(app.renderer);
}

void update(void) {
    next_gen(automaton);
}

int main(void) {
    memset(&app, 0, sizeof(struct app));
    initSDL();
    automaton = new_automaton(GWIDTH, GHEIGHT);
    
    srand(55);

    for(int i = 20; i < GWIDTH-40; i++){
        for(int j = 20; j < GHEIGHT-40; j++){
            automaton->matrix[i][j] = ((float) (rand()%100))/100;
        }
    }
    
    //atexit(cleanup);

    while(TRUE) {
        handle_input();
        render();
        update();
        total_frames++;
    }
    printf("finished\n");

    return 0;
}
