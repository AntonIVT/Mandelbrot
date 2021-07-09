SFML   = -lsfml-graphics -lsfml-window -lsfml-system
DEBUG  = -g  -fsanitize=address -fsanitize=alignment -fsanitize=bool -fsanitize=bounds -fsanitize=enum -fsanitize=float-cast-overflow -fsanitize=float-divide-by-zero -fsanitize=integer-divide-by-zero -fsanitize=leak -fsanitize=nonnull-attribute -fsanitize=null -fsanitize=object-size -fsanitize=return -fsanitize=returns-nonnull-attribute -fsanitize=shift -fsanitize=signed-integer-overflow -fsanitize=undefined -fsanitize=unreachable -fsanitize=vla-bound -fsanitize=vptr 


all: main
	
main:	
	g++ -o mandelbrot main.cpp $(SFML) -mavx2 -fopenmp -O3

debug:
	g++ -o mandelbrot_debug $(DEBUG) main.cpp $(SFML) -mavx2 -fopenmp -O3
