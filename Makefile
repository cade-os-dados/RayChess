all:
	g++ src/main.cpp -o app.exe -O1 -Wall -Wno-missing-braces -I ./include/ -I ./src/ -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm