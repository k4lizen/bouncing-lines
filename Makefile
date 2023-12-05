build:
	g++ ./src/main.cpp -o main.exe

run:
	g++ ./src/main.cpp -o main.exe
	./main.exe

image:
	g++ ./src/main.cpp -o main.exe
	main.exe > image.ppm