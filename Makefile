all: Mandelbrot

Mandelbrot:
	g++ -o Mandelbrot main.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	
