struct automaton {
    float** matrix;
    int width;
    int height;
};

void next_gen(struct automaton* automaton);
struct automaton* new_automaton(int width, int height);