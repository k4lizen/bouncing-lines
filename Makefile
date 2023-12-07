build:
	g++ ./src/main.cpp -o main.exe -Wall

run:
	g++ ./src/main.cpp -o main.exe -Wall -O3 -march=native
	./main.exe