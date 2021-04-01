all: main

clean:
	g++ -o Mandelbrot clean.cpp -lsfml-graphics -lsfml-window -lsfml-system -fopenmp -mavx2 -O3
	
main:	
	g++ -o Mandelbrot main.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -fopenmp -O3
