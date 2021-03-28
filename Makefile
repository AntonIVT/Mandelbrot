all: Mandelbrot

Mandelbrot:
	g++ -o Mandelbrot Mandelbrot.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	
main:
	g++ -o Mandelbrot main.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
