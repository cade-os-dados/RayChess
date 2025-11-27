RAYLIB = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
WARNINGS = -Wall -Wno-missing-braces
INCLUDE = -I ./include/ -I ./src/ -L ./lib/
BOOST = -lboost_system
OPENSSL = -lssl -lcrypto

all:
	g++ src/main.cpp -o app.exe -O1 -Wall -Wno-missing-braces -I ./include/ -I ./src/ -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
linux:
	g++ src/main.cpp -o app -O1  -I ./include/ -I ./src/ -L ./lib/ $(RAYLIB)
online:
	g++ src/main_online.cpp -o app -O1 $(INCLUDE) $(RAYLIB) $(BOOST) $(OPENSSL)
