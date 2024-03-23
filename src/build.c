#include <stdio.h>
#include <stdlib.h>
#include<time.h>

#include "consts.h"
#include "structs.h"

#include "automaton.h"

struct automaton* tomato;
FILE *pipout;
unsigned char fra[GHEIGHT][GWIDTH][3] = {0};
unsigned totalf = 0;


void save(void) {
    for(int i = 0; i < GWIDTH; i++) {
        for(int j = 0; j < GHEIGHT; j++) {
            fra[j][i][0] = tomato->matrix[i][j]*122;
            fra[j][i][1] = tomato->matrix[i][j]*242;
            fra[j][i][2] = tomato->matrix[i][j]*255;
        }
    }
    fwrite(fra, 1, GHEIGHT*GWIDTH*3, pipout);
}

int main(void) {
    tomato = new_automaton(GWIDTH, GHEIGHT);
    
    srand(55);
    pipout = popen("ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s 1280x720 -r 25 -i - -f mp4 -q:v 5 -an -vcodec mpeg4 output.mp4", "w");

    for(int i = 20; i < GWIDTH-40; i++){
        for(int j = 20; j < GHEIGHT-40; j++){
            tomato->matrix[i][j] = ((float) (rand()%100))/100;
        }
    }

    while(TRUE) {
        save();
        next_gen(tomato);
        totalf++;

        if(totalf >= FRAMES) break;
    }
    fflush(pipout);
    pclose(pipout);

    return 0;
}
