
FLAGS = -lX11 -lXext -lcrypto -lXtst -g1 -Ofast -std=c++17
CC = g++

server:
	$(CC) server/*.cpp $(FLAGS) -o ldxs 

client:
	$(CC) client/*.cpp $(FLAGS) -o ldxc 

all: server client
