#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() and alarm() */
#include <errno.h>      /* for errno and EINTR */
#include <signal.h>     /* for sigaction() */
#include <pthread.h>     /* for sigaction() */

#include <iostream>
#include <sstream> 
#include <fstream>
#include <vector>
#include <cassert>

#define TIMEOUT_SECS    5       /* Seconds between retransmits */
#define MAX_NEIGHBORS   3      /* Assuming topology is fixed*/
#define MAX_DIST        1000    /* when no route exist; dist is considered 1000*/
#define UNKNOWN_HOP		'X'		/*when node is unreachable; next hop is not known*/
#define ECHOMAX         255     /* Longest string to echo */
#define MAXTRIES        2       /* Tries before giving up sending */

struct Element_Dist_Vector{
        char dest;
        int dist;
};

struct Distance_Vector{
        char sender;
        int no_of_neighbors;
        struct Element_Dist_Vector element_Dist_Vector[MAX_NEIGHBORS];
};

struct Routing_Table{
    char *destination;
    int next_hop;
    int distance;
};

typedef struct Distance_Vector Dist_Vect;

struct config{
    int     node_ID = 0;
    std::string name;
    int     control_port = 0;
    int     data_port = 0;
    std::vector<int> neighbours;
    struct Routing_Table routing_table[MAX_NEIGHBORS];
};
std::vector<struct config> config;

int tries=0;
unsigned char *advertise_contents;
pthread_mutex_t bellman_mutex, update_mutex, adv_mutex;

int Update_Routing_Table(Dist_Vect *dist_vect);    /*Updates Routing table after receiving distance vector from neighbors*/
void SendDistVect(Dist_Vect send_dist_vect);   /*sends the distance vector to all neighbors*/
void Build_Distance_Vector(Dist_Vect *dist_vect); /*construct a distance vector*/
void Print_Routing_Table();     /*Prints Routing table*/
void Print_Neighbor_Table();    /*Prints Neighbor table*/
void Print_Distance_Vector(Dist_Vect *dist_vector); /*Prints Distance Vector*/
void DieWithError(std::string errorMessage); /* Error handling function */
void CatchAlarm(int ignored); /* Handler for SIGALRM */
void SendDistVect(Dist_Vect send_dist_vect); /*sends the distance vector to all neighbors*/

void parse_input(const char *filename)
{
	std::string file (filename);
	std::ifstream input_file (file);
	assert(input_file.is_open() == true);
	std::string line;
	while(getline(input_file, line))
	{
		std::cout << line << std::endl;
		std::stringstream ss(line);
		std::string item;
		std::vector<std::string> tokens;
		while (getline(ss, item, ' ')) {
        	tokens.push_back(item);
    	}
    	struct config current;
    	std::stringstream(tokens[0]) >> current.node_ID;
    	current.name = tokens[1];
    	std::stringstream(tokens[2]) >> current.control_port;
    	std::stringstream(tokens[3]) >> current.data_port;
    	for (int i = 4; i < tokens.size(); i++)	{
    		int neighbour = 0;
    		std::stringstream(tokens[i]) >> neighbour;
    		current.neighbours.push_back(neighbour);
    	}
    	config.push_back(current);

	}
}

void print_config_vector()
{
	for (int i = 0; i < config.size(); ++i)
	{
		std::cout << "Node Id  : " <<  config[i].node_ID << std::endl;
		std::cout << "Name     : " <<  config[i].name << std::endl;
		std::cout << "CP  	   : " <<  config[i].control_port << std::endl;
		std::cout << "DP 	   : " <<  config[i].data_port << std::endl;
		for (int j = 0; j < config[i].neighbours.size(); j++)
		{
			std::cout << " N" << (i+1) << " : " << config[i].neighbours[j]; 
		}
		std::cout << std::endl;
	}
}


void SendDistVect(Dist_Vect send_dist_vect){
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    struct sigaction myAction;       /* For setting signal handler */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[ECHOMAX+1];      /* Buffer for echo string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Size of received datagram */
    int j,i = 0;
    echoStringLen = sizeof(send_dist_vect);

    /* Create a best-effort datagram socket using UDP */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError(std::string("socket() failed"));

    /* Set signal handler for alarm signal */
    myAction.sa_handler = CatchAlarm;
    if (sigfillset(&myAction.sa_mask) < 0) /* block everything in handler */
        DieWithError("sigfillset() failed");
    myAction.sa_flags = 0;

    if (sigaction(SIGALRM, &myAction, 0) < 0)
        DieWithError("sigaction() failed for SIGALRM");

 	for (int i = 0; i < config.size(); ++i){

 	for(j=0;j<config[i].neighbours.size();j++){
                char sender, dest,next_hop;
                int no_of_neighbors, dist;
				std::cout << "Sending distance vector to neighbor " << config[i].neighbours[j] << std::endl;
                /* Construct the server address structure */
                memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
                echoServAddr.sin_family = AF_INET;
                //echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
                //echoServAddr.sin_port = htons(echoServPort);       /* Server port */

                fromSize = sizeof(fromAddr);
                /* Send the string to the server */
                if (sendto(sock, (void *)&send_dist_vect, (1024+sizeof(send_dist_vect)), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) == -1)
                        DieWithError("sendto() sent a different number of bytes than expected");

                /* Get a response */
                alarm(TIMEOUT_SECS);        /* Set the timeout */
                while ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize)) < 0){
					if (errno == EINTR)     /* Alarm went off  */
					{
							if (tries < MAXTRIES)      /* incremented by signal handler */
							{
								printf("while sending, timed out, %d more tries...\n",MAXTRIES-tries);

								if (sendto(sock, (void *)&send_dist_vect, (1024+sizeof(send_dist_vect)), 0, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) == -1)
										DieWithError("sendto() failed");
								alarm(TIMEOUT_SECS);
							}
							break;
					}
					printf("waiting to receive");
				}
				/* recvfrom() got something --  cancel the timeout */
				alarm(0);
				/*if send is not successful even after timeout; then go for sending to next neighbor*/
				if(respStringLen<0)
						continue;
        }
		close(sock);
        return;
    }
}

int Update_Routing_Table(Dist_Vect *dist_vect){
        int j=0,dist_to_nei=0,i=0,Update_Flag=0;
        for(j=0;j<config->neighbors;j++){                    //check for link cost to this neighbor
                if(config->neighbors[j].dest==dist_vect->sender){
                        dist_to_nei=config->neighbors[j].dist;
                }
        }
        for(i=0;i<MAX_NEIGHBORS;i++){                                           //check for each node if there is a new value
        for(j=0;j<MAX_NEIGHBORS;j++){
               if(config->routing_table[i].dest==dist_vect->element_Dist_Vector[j].dest){
                        /*Update if next_hop is same as sender but dist is changed to dest 
                            OR Currently node is unreachable but reachable from sender
                            OR  Current distance dest node is greater than (dist to dest from sender+dist to sender)
                        */
                        if((config->routing_table[i].dist==MAX_DIST && dist_vect->element_Dist_Vector[j].dist!=MAX_DIST) 
                            || (config->routing_table[i].dist>(dist_vect->element_Dist_Vector[j].dist+dist_to_nei))
//                      || (info_config->routing_table[i].next_hop==dist_vect->sender && info_config->routing_table[i].dist!=dist_vect->element_Dist_Vector[j].dist)
                            ){
                                config->routing_table[i].dist=dist_vect->element_Dist_Vector[j].dist + dist_to_nei;
                                config->routing_table[i].next_hop=dist_vect->sender;
                                Update_Flag=1;
                       }

                }
        }
        }
return Update_Flag;
}
void Build_Distance_Vector(Dist_Vect *dist_vect,Info_Config *info_config){
        int j=0;
        dist_vect->sender=info_config->node_name;
        dist_vect->no_of_neighbors=info_config->no_of_neighbors;
        for(j=0;j<MAX_NEIGHBORS;j++){
                dist_vect->element_Dist_Vector[j].dest=config->routing_table[j].dest;
                dist_vect->element_Dist_Vector[j].dist=config->routing_table[j].dist;
        }
}
void Print_Distance_Vector(Dist_Vect *dist_vect){
        int j=0;
        printf("\nSender:%c", dist_vect->sender);
        printf("\nNo_of_neighbors:%d", dist_vect->no_of_neighbors);
        printf("\ndest\tdist");
        for(j=0;j<MAX_NEIGHBORS;j++){
                printf("\n%c\t%d",dist_vect->element_Dist_Vector[j].dest,dist_vect->element_Dist_Vector[j].dist);
        }
}
void Populate_From_Config(FILE *fp,Info_Config *info_config){

        /*Get dest, dist, ip, port_no fields for each neighbor*/
        // while(fgets(value1,255,fp) && fgets(value2,255,fp) && fgets(value3,255,fp) && fgets(value4,255,fp) !=NULL){
           i=0;
           for(j=0;j<MAX_NEIGHBORS;j++){
                if(nodes[i]==info_config->node_name){   /*if information is about this node itself; don't store it in routing/neighbor table*/
                        i++;
                        j--;
                        continue;
                }else if(nodes[i]==value1[0]){  /*If node is there in config file; populate routing/ neighbor table*/
                        //add to neighbors if valid distance for destination
                        if(atoi(value2) > 0 && atoi(value2) < MAX_DIST){
                            info_config->neighbors[no_of_neighbors].dest=value1[0];
                            info_config->neighbors[no_of_neighbors].dist=atoi(value2);
                            strcpy(info_config->neighbors[no_of_neighbors].ip,value3);
                            info_config->neighbors[no_of_neighbors].port_no=atoi(value4);
                            no_of_neighbors++;
                            //add to initial routing table
                            info_config->routing_table[j].dest=value1[0];
                            info_config->routing_table[j].dist=atoi(value2);
                            info_config->routing_table[j].next_hop=nodes[i];
                        }else{
                            info_config->routing_table[j].next_hop=nodes[i];
                            info_config->routing_table[j].dist=MAX_DIST;
                            info_config->routing_table[j].next_hop=UNKNOWN_HOP;
                        }
                        i++;
                }else{                          /*If node is not in config file; assign dest with max_dist and 'X'(unknown) next_hop*/
                        info_config->routing_table[j].dest=nodes[i];
                        if(info_config->routing_table[j].dist>MAX_DIST || info_config->routing_table[j].dist==0 || info_config->routing_table[j].dist<0){
                                info_config->routing_table[j].dist=MAX_DIST;
                                info_config->routing_table[j].next_hop=UNKNOWN_HOP;
                        }
                        i++;
                }
        }
        config->no_of_neighbors=no_of_neighbors;
        }
        fclose(fp);
}
void Print_Routing_Table(Info_Config *info_config){
        int j=0;
        printf("\ndest\tdist\tnext_hop");
        for(j=0;j<MAX_NEIGHBORS;j++){
                printf("\n%c\t%d\t%c",info_config->routing_table[j].dest,info_config->routing_table[j].dist,info_config->routing_table[j].next_hop);
        }
}
void Print_Neighbor_Table(Info_Config *info_config){
                printf("\nNeighbor Table:");
                int j=0;
                printf("\nNeighbor\tdist\tip\tport_no");
                for(j=0;j<config->no_of_neighbors;j++){
                        printf("\n%c\t\t%d\t%s\t%d",config->neighbors[j].dest,config->neighbors[j].dist,config->neighbors[j].ip,info_config->neighbors[j].port_no);
                }
}


void DieWithError(conststd::string errorMessage){
    std::cerr << errorMessage << std::endl;
    exit(1);   
}

void CatchAlarm(int ignored){
    tries += 1;
}         

int main(int argc, char const **argv)
{
	parse_input(argv[1]);
	Dist_Vect send_dist_vect;
	print_config_vector();
	        /*Build initial distance vector*/
        // Build_Distance_Vector(&send_dist_vect,);
  
		printf("\n/**********************************************INITIAL SEND******************************************************************************/");
        /*send initial dist_vect to all neighbors*/
		SendDistVect(send_dist_vect);

	return 0;
}