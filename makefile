Terrain: Terrain.cpp FORCE
	g++ Terrain.cpp -o terrain -lGL -lGLU -lglut
	./terrain

.PHONY: FORCE
FORCE: 

clean: 
	-rm -rf Terrain