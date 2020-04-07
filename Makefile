CC = g++

#SRCS = ../*.cpp
RO_SRCS = main.cpp

clique_find:
	$(CC) -DDEBUG -O3 --std=c++11 -pthread -o $@ $(RO_SRCS)

clean:
	rm clique_find
