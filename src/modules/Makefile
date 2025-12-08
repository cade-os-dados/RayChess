LIB = -lraylib
LOCATE_LIB = -L../../lib
STD = -std=c++20
INCLUDE = -I../../include -I.
FLAG = -fmodules-ts -x
MODULES = c++ coord.cppm board.cppm move.cppm
WINDOWS = -lopengl32 -lgdi32 -lwinmm

M = coord board move
HEADERS = vector iostream tuple

compile_headers:
	@for h in $(HEADERS); do \
		echo "Compilando system header $$h..."; \
		g++ $(STD) $(FLAG) c++-system-header $$h; \
	done

compile_modules:
	@for m in $(M); do \
		echo "Compilando módulo $$m..."; \
		g++ -c $(STD) $(LOCATE_LIB) $(INCLUDE) -fmodules-ts -x c++ $$m.cppm; \
		rm $$m.o; \
	done

compile: compile_headers compile_modules

all:
	g++ move.cpp $(STD) $(INCLUDE) $(LOCATE_LIB) $(FLAG) $(MODULES) -o main $(LIB) $(WINDOWS)

# coord:
# 	g++ -c $(STD) $(LOCATE_LIB) $(INCLUDE) -fmodules-ts -x c++ coord.cppm
# 	rm coord.o
# board:
# 	g++ -c $(STD) $(LOCATE_LIB) $(INCLUDE) -fmodules-ts -x c++ board.cppm
# 	rm board.o
# move:
# 	g++ -c $(STD) $(LOCATE_LIB) $(INCLUDE) -fmodules-ts -x c++ move.cppm
# 	rm move.o

# std: vector iostream tuple

# vector:
# 	g++ $(STD) $(FLAG) c++-system-header vector
# iostream:
# 	g++ $(STD) $(FLAG) c++-system-header iostream
# tuple:
# 	g++ $(STD) $(FLAG) c++-system-header tuple
