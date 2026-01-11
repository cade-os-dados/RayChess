LINUX = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
WINDOWS = -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32
WARNINGS = -Wall -Wno-missing-braces
INCLUDE = -I ./include/ -I ./src/ -L ./lib/
BOOST = -lboost_system
OPENSSL = -lssl -lcrypto

all:
	g++ src/main.cpp -o app.exe -O1 -Wall -Wno-missing-braces -I ./include/ -I ./src/ -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
linux:
	g++ src/main.cpp -o app -O1  -I ./include/ -I ./src/ -L ./lib/ $(LINUX)
online:
	g++ src/main_online.cpp -o app -O1 $(INCLUDE) $(LINUX) $(BOOST) $(OPENSSL)
online_geral:
	g++ src/main_online_geral.cpp -o app -O1 $(INCLUDE) $(LINUX) $(BOOST) $(OPENSSL)
online_client:
	g++ src/main_online_client.cpp -o client $(INCLUDE) $(LINUX) $(BOOST) $(OPENSSL)
client:
	g++ src/client.cpp -o client $(OPENSSL) $(BOOST)
online_geral_windows:
	g++ src/main_online_geral.cpp -o app -O1 $(INCLUDE) $(WINDOWS) $(OPENSSL)