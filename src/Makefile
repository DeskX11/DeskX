.DEFAULT_GOAL := all
FLAGS = -lX11 -lXext -lcrypto -lXtst -Ofast -std=c++17
dxs:
	g++ server/*.cpp $(FLAGS) -o dxs 
dxc:
	g++ client/*.cpp $(FLAGS) -o dxc 
all: dxs dxc
