VERSION = 2.0.2
INCLUDE = -I./src
SDL2  = $(shell sdl2-config --cflags --libs)
FLAGS = $(INCLUDE) -Ofast -lpthread $(SDL2) -fno-exceptions -std=c++17 -DVERSION="\"$(VERSION)\""

ifeq ($(shell uname -s), Linux)
	DISPLAY = ./src/display/x11.cpp ./src/display/wayland.cpp ./src/display/wayland/*.cpp
	LIBS 	= -lX11 -lXext -lXtst `pkg-config --cflags --libs libportal libpipewire-0.3`
else ifeq ($(shell uname -s), Darwin)
	DISPLAY = ./src/display/osx.cpp
	LIBS 	=
else
	DISPLAY = ./src/display/d3d11.cpp
	LIBS 	= -ld3d11
endif

all:
	g++ ./src/*.cpp ./src/codec/*.cpp ./src/client/*.cpp $(DISPLAY) $(FLAGS) $(LIBS) -o deskx

dpkg:
	cd deb; ./build.sh $(VERSION)