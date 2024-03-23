//int rules[3] = {2, 3, 3};
//
//int outer_area[3][3] = {
//    {1,1,1},
//    {1,0,1},
//    {1,1,1}
//};
// max for underpopulation, min_breading, min for overpopulation
float STEP = 0.03;

float INNER_RADIUS = 10;
float OUTER_RADIUS = 20;

float BIRTH_INTERVAL[2] = {0.278, 0.365};
float DEATH_INTERVAL[2] = {0.267, 0.445};


int** inner_area;
int** outer_area;

int inner_total;
int outer_total;

#include <malloc.h>
#include <math.h>
#include <omp.h>

#include "automaton.h"
#include "consts.h"

static float sig1(float x, float a, float step) {
    return (1 / (1 + powf(EULER_NUMBER_F, -(x - a)*4 / step)));
}

static float sig2(float x, float a, float b, float step) {
    return sig1(x, a, step)*(1 - sig1(x, b, step));
}

static float sigm(float x, float y, float m, float step) { 
    return x*(1 - sig1(m, 0.5, step)) + y * sig1(m, 0.5, step);
}

static float t_function(float n, float m, float step) {
    return sig2(n, sigm(BIRTH_INTERVAL[0], DEATH_INTERVAL[0], m, step), sigm(BIRTH_INTERVAL[1], DEATH_INTERVAL[1], m, step), step);
}

// float normalize(float val) {
//(val > 1) return 1;
//(val < 0) return 0;
//turn val;
//

struct automaton* new_automaton(int width, int height) {


    struct automaton* automaton = (struct automaton*) malloc(sizeof(struct automaton));

    automaton->width = width;
    automaton->height = height;

    automaton->matrix = (float**) malloc(sizeof(float*) * automaton->width);
    for(int i = 0; i < automaton->width; i++) 
        automaton->matrix[i] = calloc(automaton->height, sizeof(float));


    /* SET OUTER AND INNER CONVOLUTION MATRICES */

    outer_area = (int**) malloc(sizeof(int*) * (OUTER_RADIUS*2+1));
    for(int i = 0; i < OUTER_RADIUS*2+1; i++)
        outer_area[i] = calloc(OUTER_RADIUS*2+1, sizeof(int));
    inner_area = (int**) malloc(sizeof(int*) * (INNER_RADIUS*2+1));
    for(int i = 0; i < INNER_RADIUS*2+1; i++)
        inner_area[i] = calloc(INNER_RADIUS*2+1, sizeof(int));


    float inner_squared = (INNER_RADIUS * INNER_RADIUS);
    float outer_squared = OUTER_RADIUS * OUTER_RADIUS;
    
    int gx, gy, inner;
    float mininner, min, max;
    
    for(int i = -OUTER_RADIUS; i < OUTER_RADIUS; i++) {
        min = -sqrtf(outer_squared - (i*i));
        max = -min;

        inner = FALSE;
        if(i > -INNER_RADIUS && i < INNER_RADIUS) {
            mininner = -sqrtf(inner_squared - (i*i));
            inner = TRUE;
        }
        
        for(int j = min; j < max; j++) {            
            gx = OUTER_RADIUS + i;
            gy = OUTER_RADIUS + j;

            if(!inner || gy < OUTER_RADIUS + mininner || gy > OUTER_RADIUS -mininner) {
                outer_area[gx][gy] = 1;
                outer_total++;
            }
        }
    }
    
    for(int i = -INNER_RADIUS; i < INNER_RADIUS; i++) {
        min = -sqrtf(inner_squared - (i*i));
        max = -min;

        for(int j = min; j < max; j++) {            
            gx = INNER_RADIUS + i;
            gy = INNER_RADIUS + j;
            
            inner_area[gx][gy] = 1;
            inner_total++;
        }
    }

    /* END SET CONVOLUTION MATRICES */

    return automaton;
}

static float normalize(float a) {
    if(a < 0) return 0;
    if(a > 1) return 1;
    return a;
}

static float get_state(struct automaton automaton, int x, int y) {
    float innerc = 0;
    float outerc = 0;
    

    // outer radius
    for(int i = -OUTER_RADIUS; i < OUTER_RADIUS; i++) {
        if(x + i < 0 || x + i >= automaton.width) continue;
        for(int j = -OUTER_RADIUS; j < OUTER_RADIUS; j++) {
            if(y + j < 0 || y + j >= automaton.height) continue;

            int outer_area_val = outer_area[i+(int) OUTER_RADIUS][j+(int) OUTER_RADIUS];
            outerc += automaton.matrix[x + i][y + j] * outer_area_val;
        }
    }

    // outer radius
    for(int i = -INNER_RADIUS; i < INNER_RADIUS; i++) {
        if(x + i < 0 || x + i >= automaton.width) continue;
        for(int j = -INNER_RADIUS; j < INNER_RADIUS; j++) {
            if(y + j < 0 || y + j >= automaton.height) continue;

            int inner_area_val = inner_area[i+(int) INNER_RADIUS][j+(int) INNER_RADIUS];
            outerc += automaton.matrix[x + i][y + j] * inner_area_val;
        }
    }

    float old = automaton.matrix[x][y];

    float m = innerc / inner_total;
    float n = outerc / outer_total;

    //if(m < 0.5 && n > 0.25 && n < 0.33) return sigmoid(old + BIRTH_RATE);
    //if(m > 0.5 && n > 0.35 && n < 0.60) return sigmoid(old + BIRTH_RATE);
    //if(m < 0.8 && n > 0.20 && n < 0.33) return normalize(old + BIRTH_RATE);
    //if(m > 0.5 && n > 0.30 && n < 0.70) return normalize(old + BIRTH_RATE);

    return normalize(old + t_function(n, m, STEP)-0.2);
}

static void free_matrix(struct automaton* automaton) {
    for(int i = 0; i < automaton->width; i++) free(automaton->matrix[i]);
    
    free(automaton->matrix);
}

void next_gen(struct automaton* automaton) {
    // allocate next gen matrix
    float** new_matrix = (float**) malloc(sizeof(float*) * automaton->width);
    for(int i = 0; i < automaton->width; i++) 
        new_matrix[i] = (float*) malloc(sizeof(float) * automaton->height);

    for(int i = 0; i < automaton->width; i++)
        for(int j = 0; j < automaton->height; j++)
            new_matrix[i][j] = get_state(*automaton, i, j);
    
    free_matrix(automaton);
    automaton->matrix = new_matrix;
}