test:
	gcc src/automaton.c src/main.c -Wall -Wextra -pedantic -std=c99 -lm -lSDL2 -fopenmp -o main.out

build: 
	gcc src/automaton.c src/build.c -Wall -Wextra -pedantic -std=c99 -lm -lSDL2 -fopenmp -o b.out && ./b.out && rm b.out

run:
	./main.out

clean:
	rm main.out