all: proxy_server

proxy_server: http_proxy_server.cpp
	clang++ -lpthread -g -o proxy http_proxy_server.cpp -lm
	
clean:
	rm -f *.o proxy
