/*--------------------------------------------------------------------*/
/* functions to connect clients and server */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>

#include <stdlib.h>

#define MAXNAMELEN 256
/*--------------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/* prepare server to accept requests
 returns file descriptor of socket
 returns -1 on error
 */
int startserver() {
	int sd; /* socket descriptor */

	char * servhost; /* full name of this host */
	ushort servport; /* port assigned to this server */

	struct sockaddr_in sin_serv;
	    struct hostent *hp;
	    socklen_t len;
	    char servaddr[100];
	    int rv;
	    sd = socket(AF_INET, SOCK_STREAM, 0);
		switch(sd)
		{
		case -1:
		return -1;
		break;
		}

	    bzero(&sin_serv, sizeof(sin_serv));
	    sin_serv.sin_family = AF_INET;
	    sin_serv.sin_port = htons(0);
	    sin_serv.sin_addr.s_addr = htonl(INADDR_ANY);

	    if( bind(sd, (struct sockaddr *)&sin_serv, sizeof(sin_serv)) ){
		close(sd);
		return -1;
	    }
	/*
	 FILL HERE
	 create a TCP socket using socket()
	 */


	/*
	 FILL HERE
	 bind the socket to some port using bind()
	 let the system choose a port
	 */

	/* we are ready to receive connections */
	listen(sd, 5);

	servhost = malloc(MAXNAMELEN);
	int br;
	br = gethostname(servhost, MAXNAMELEN);
	switch(br)
	{
	case -1:
		return -1;
		break;
	}	    
	
	if( (hp = gethostbyname(servhost)) < 0 )
		return -1;
	    strcpy(servhost, hp->h_name);

	    bzero(&sin_serv, sizeof(sin_serv));
	    len = sizeof(sin_serv);
	int vr;
	vr = 	getsockname(sd, (struct sockaddr *)&sin_serv, &len);
	switch(vr)
	case -1:
	{
		close(sd);
		return -1;
		break;
	}
	    servport = ntohs(sin_serv.sin_port);

	/*
	 FILL HERE
	 figure out the full host name (servhost)
	 use gethostname() and gethostbyname()
	 full host name is remote**.cs.binghamton.edu
	 */


	/*
	 FILL HERE
	 figure out the port assigned to this server (servport)
	 use getsockname()
	 */


	/* ready to accept requests */
	printf("admin: started server on '%s' at '%hu'\n", servhost, servport);
	free(servhost);
	return (sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/*
 establishes connection with the server
 returns file descriptor of socket
 returns -1 on error
 */
int hooktoserver(char *servhost, char *servport) {
	int sd; /* socket descriptor */

	ushort clientport; /* port assigned to this client */

	/*
	 FILL HERE
	 create a TCP socket using socket()
	 */
	 sd = socket(PF_INET, SOCK_STREAM, 0);
  	*servport = '\0';

	switch(sd)
	{
		case -1:
		return -1;
		break;
	}
 //if (sd < 0){
//	 return -1;
 //}
	/*
	 FILL HERE
	 connect to the server on 'servhost' at 'servport'
	 use gethostbyname() and connect()
	 */
	 struct sockaddr_in sin;
  	bzero(&sin, sizeof(struct sockaddr_in));
  	sin.sin_family = AF_INET;
  	sin.sin_port = htons(atoi(servport));

  struct hostent *hostEnt;
  if(hostEnt = gethostbyname(servhost))
      memcpy(&sin.sin_addr, hostEnt->h_addr, hostEnt->h_length);
  else {
      fprintf(stderr, "Can't get host by name.\n");
      close(sd);
      return -1;
  }

	int dg;
	dg = connect(sd, (struct sockaddr*)&sin, sizeof(sin));
	switch(dg)
	{
		case -1:
		      fprintf(stderr, "Can't connect to server.\n");
		      close(sd);
      		      return -1;
	}

	/*
	 FILL HERE
	 figure out the port assigned to this client
	 use getsockname()
	 */


	/* succesful. return socket descriptor */
	printf("admin: connected to server on '%s' at '%s' thru '%hu'\n", servhost,
			servport, clientport);
	printf(">");
	fflush(stdout);
	return (sd);
}
/*----------------------------------------------------------------*/

/*----------------------------------------------------------------*/
int readn(int sd, char *buf, int n) {
	int toberead;
	char * ptr;

	toberead = n;
	ptr = buf;
	while (toberead > 0) {
		int byteread;

		byteread = read(sd, ptr, toberead);
		switch(byteread)
		{
			case -1:
				perror("read");
				return(0);
				break;
			case 0:
				perror("read");
				return(0);
				break;
		}

		toberead -= byteread;
		ptr += byteread;
	}
	return (1);
}

char *recvtext(int sd) {
	char *msg;
	long len;

	/* read the message length */
	if (!readn(sd, (char *) &len, sizeof(len))) {
		return (NULL);
	}
	len = ntohl(len);

	/* allocate space for message text */
	msg = NULL;
	if (len > 0) {
		msg = (char *) malloc(len);
		if (!msg) {
			fprintf(stderr, "error : unable to malloc\n");
			return (NULL);
		}

		/* read the message text */
		if (!readn(sd, msg, len)) {
			free(msg);
			return (NULL);
		}
	}

	/* done reading */
	return (msg);
}

int sendtext(int sd, char *msg) {
	long len;

	/* write lent */
	len = (msg ? strlen(msg) + 1 : 0);
	len = htonl(len);
	write(sd, (char *) &len, sizeof(len));

	/* write message text */
	len = ntohl(len);
	if (len > 0)
		write(sd, msg, len);
	return (1);
}
/*----------------------------------------------------------------*/

