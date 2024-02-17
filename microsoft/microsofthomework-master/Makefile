
default: client server

client: client.cpp
		echo "make client"
		g++ -g client.cpp -o client -lpthread -lstdc++ -lc


server : server.cpp
		echo "make server"
		g++ -g server.cpp -o server -lpthread -lstdc++ -lc


.PHONY: all
all: client server


