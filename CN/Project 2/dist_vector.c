#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() and alarm() */
#include <errno.h>      /* for errno and EINTR */
#include <signal.h>     /* for sigaction() */

#include <iostream>
#include <sstream> 
#include <fsstream>
#include <vector>
#include <cassert>

#define TIMEOUT_SECS    5       /* Seconds between retransmits */
#define MAX_NEIGHBORS   3      /* Assuming topology is fixed*/
#define MAX_DIST        1000    /* when no route exist; dist is considered 1000*/
#define UNKNOWN_HOP		'X'		/*when node is unreachable; next hop is not known*/
#define ECHOMAX         255     /* Longest string to echo */
#define MAXTRIES        2       /* Tries before giving up sending */

struct config{
    int	    node_ID;
    std::string node_name;
	char	*name;
	int 	control_port;
	int 	data_port
	int 	neighbor1;
	int 	neighbor2;
	int 	neighbor3;
};

struct config config;

void parse_input(char *filename)
{
	std::string file (filename);
	std::ifstream input_file (file);
	assert(input_file.is_open() == true);
	std::string line;
	while(getline(input_file, line))
	{
		std::cout << line << std::endl;
	}
}

int main(int argc, char const *argv[])
{
	parse_input(argv[1]);
	return 0;
}