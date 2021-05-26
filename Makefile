FLAGS = -lX11 -lXext -lcrypto -lXtst -lpthread -Ofast -std=c++17

client:
	g++ src/Tools.cpp src/client/*.cpp src/Network/*.cpp $(FLAGS) -o dxc 
server:
	g++ src/Tools.cpp src/server/*.cpp src/Network/*.cpp $(FLAGS) -o dxs
