VERSION = 2.0.1
FLAGS = -I./src -Ofast -lpthread `sdl2-config --cflags --libs` -fno-exceptions -std=c++17 -DVERSION="\"$(VERSION)\""

ifeq ($(shell uname -s), Linux)
	DISPLAY = ./src/display/x11.cpp ./src/display/wayland.cpp ./src/display/wayland/*.cpp
	LIBS 	= -lX11 -lXext -lXtst `pkg-config --cflags --libs libportal libpipewire-0.3`
	NAME	= linux
else ifeq ($(shell uname -s), Darwin)
	DISPLAY = ./src/display/osx.cpp
	LIBS 	=
	NAME	= osx
else
	DISPLAY = ./src/display/win.cpp
	LIBS 	=
	NAME	= win
endif

all:
	g++ ./src/*.cpp ./src/codec/*.cpp ./src/client/*.cpp $(DISPLAY) $(FLAGS) $(LIBS) -o deskx-$(NAME)

deb:
	cd deb; ./build.sh $(VERSION)