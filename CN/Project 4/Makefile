all:	ctClient	nodeServer

ctClient:	ctClient.o
	g++ -pthread -g -std=c++11 Node.o ctClient.o -o ctClient

ctClient.o:	ctClient.cpp
	g++ -std=c++11 -c Node.cpp ctClient.cpp
	
nodeServer: nodeServer.o
	g++ -pthread -std=c++11 Node.o nodeServer.o -o nodeServer
	
nodeServer.o:	nodeServer.cpp
	g++ -std=c++11 -c Node.cpp nodeServer.cpp

clean:
	rm -f *.o ctClient nodeServer
