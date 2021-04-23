.DEFAULT_GOAL := all
FLAGS = -lX11 -lXext -lcrypto -lXtst -g1 -Ofast -std=c++17
CC = g++

ldxs:
	$(CC) server/*.cpp $(FLAGS) -o ldxs 

ldxc:
	$(CC) client/*.cpp $(FLAGS) -o ldxc 

all: ldxs ldxc
