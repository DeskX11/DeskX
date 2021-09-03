CLIENT_SRC = src/Tools.cpp src/client/Actions.cpp src/client/main.cpp src/Network/*.cpp
FLAGS = -lpthread -Ofast -std=c++17

client:
ifeq ($(shell uname -s), Darwin)
	g++ $(CLIENT_SRC) src/client/SDL.cpp $(FLAGS) `sdl2-config --cflags --libs` -o dxc 
else
	g++ $(CLIENT_SRC) src/client/x11.cpp $(FLAGS) -lX11 -lXext -lXtst -o dxc 
endif
server:
	g++ src/Tools.cpp src/server/*.cpp src/Network/*.cpp $(FLAGS) -lX11 -lXext -lXtst -o dxs
keyboard:
	g++ src/KeyConverter.cpp -o keys -std=c++17

deb-client:
	cd deb; ./build.sh client
deb-server:
	cd deb; ./build.sh server
