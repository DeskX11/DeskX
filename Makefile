VERSION = 1.3
SRC = src/Net.cpp src/Args.cpp src/Events.cpp src/codec/*.cpp
X11 = -lX11 -lXext -lXtst
FLAGS = -lpthread -Ofast -std=c++17 -DVERSION="\"$(VERSION)\""

client:
ifeq ($(shell uname -s), Linux)
	g++ $(SRC) src/Client.cpp src/client/Actions.cpp src/client/x11.cpp $(FLAGS) ${X11} -o dxc 
else
	g++ $(SRC) src/Client.cpp src/client/Actions.cpp src/client/SDL.cpp $(FLAGS) `sdl2-config --cflags --libs` -o dxc 
endif
server:
	g++ $(SRC) src/Server.cpp src/server/*.cpp $(FLAGS) ${X11} -o dxs
keyboard:
	g++ src/Keyboard.cpp -o keys -std=c++17

deb-client:
	cd deb; ./build.sh client $(VERSION)
deb-server:
	cd deb; ./build.sh server $(VERSION)
