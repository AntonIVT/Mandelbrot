all: slow


slow:
	g++ -o slow slow.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	./slow

fast:
	g++ -o fast fast.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	./fast

fast_float:
	g++ -o fast_float fast_float.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	./fast_float