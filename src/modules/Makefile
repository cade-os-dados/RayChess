########### MAKEFILE FOR CLANG++ MODULES

HEADERS = iostream vector tuple
COMPILER = clang++
STD = -std=c++23
MODULES = coord board move  
INCLUDE = -I../../include
LIB = -L../../lib
PCM = -fmodule-file=pcm/iostream.pcm\
	-fmodule-file=pcm/tuple.pcm\
	-fmodule-file=pcm/vector.pcm\
	-fmodule-file=coord=pcm/coord.pcm\
	-fmodule-file=board=pcm/board.pcm\
	-fmodule-file=move=pcm/move.pcm

TEST = pcm/iostream.pcm pcm/tuple.pcm pcm/vector.pcm pcm/coord.pcm pcm/board.pcm pcm/move.pcm

all: mkdir stl modules main

mkdir:
	@echo "Criando diretório pcm..."
	mkdir -p pcm

stl:
	@for h in $(HEADERS); do \
		echo "Compilando system header $$h..."; \
		$(COMPILER) $(STD) -xc++-system-header --precompile $$h -o pcm/$$h.pcm; \
	done

modules:
	@for m in $(MODULES); do \
		echo "Compilando module... $$m"; \
		$(COMPILER) $(STD) $(INCLUDE) $$m.cppm $(PCM) --precompile -o pcm/$$m.pcm; \
	done

main:
	$(COMPILER) $(STD) $(LIB) $(INCLUDE) -fmodules move.cpp $(PCM) $(TEST) -o main -lraylib