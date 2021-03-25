all: slow


slow:
	g++ -o slow slow.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	./slow

fast:
	g++ -o fast fast.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	./fast

fast_cpp:
	g++ -o fast_cpp fast_cpp.cpp -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	
fast_cpp_san:
	g++ -o fast_cpp fast_cpp.cpp -fsanitize=address -lsfml-graphics -lsfml-window -lsfml-system -mavx2 -O3
	./fast_cpp
