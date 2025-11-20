RAYLIB = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
WARNINGS = -Wall -Wno-missing-braces

all:
	g++ src/main.cpp -o app.exe -O1 -Wall -Wno-missing-braces -I ./include/ -I ./src/ -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
linux:
	g++ src/main.cpp -o app -O1  -I ./include/ -I ./src/ -L ./lib/ $(RAYLIB)
