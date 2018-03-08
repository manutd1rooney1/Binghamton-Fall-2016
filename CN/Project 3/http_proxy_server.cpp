#include <vector>
#include <string>
#include <iostream>
using namespace std;
#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <string.h>

int dontcallme(int sock_descriptor);

#define NO_OF_CONN 500
#define RUNTHIS 1
#define REQ_BUFF_SIZE 10000
#define HTTP_REQUEST_BUFF_SIZE 1000
#define	SPLITED_PTR_SIZE 250
#define CACHE_FLAG 1

pthread_mutex_t thread_lock;
struct cache_object
{
	vector<string> my_req;
	vector<int> track_char;
	vector<string> my_resp;
	int contentlength;
	int size;
};

int rt = 0;
struct cache_object cache[200];
int cache_entry_count = 0;

int start_proxy_server(){
	int proxy_socket;
	char *proxy_server_host;
	ushort proxy_server_port;
	//int len;
	socklen_t len;
	struct sockaddr_in proxy_server;
	if((proxy_socket = socket(AF_INET,SOCK_STREAM,0)) == -1){
	 	perror("socket()");
	 	exit(-1);
	 }

	bzero(&proxy_server.sin_zero,8);
	proxy_server.sin_family = AF_INET;
	proxy_server.sin_port = htons(0);
	proxy_server.sin_addr.s_addr = INADDR_ANY;
	
	len = sizeof(struct sockaddr_in);

    if((bind(proxy_socket,(struct sockaddr *)&proxy_server,len)) == -1)
	 {
		perror("********** Error in Bind **********");
		exit(-1);
	 }
	if(listen(proxy_socket,NO_OF_CONN) == -1)
	{
		perror("********** Error in listen **********");
		exit(-1);
	}

	int gethostname_return = gethostname((char*)&proxy_server_host,128);

	 if(gethostname_return == -1){
	 	perror("********** Error in gethostname() **********");
	 	exit(-1);
	 }

	struct hostent *var_hostent = 0;
	struct in_addr **addr_list;
	var_hostent = gethostbyname((const char *)&proxy_server_host);
	if (!var_hostent) {
        printf("********** Error in gethostbyname() Lookup Failed **********");
        exit(-1);
    }

    int strlen_h_name = strlen(var_hostent->h_name);
	proxy_server_host = (char*)malloc(strlen_h_name+1);

	memcpy(proxy_server_host,var_hostent->h_name,strlen_h_name);
	proxy_server_host[strlen_h_name] = '\0';

	if(getsockname(proxy_socket,(struct sockaddr *)&proxy_server,&len) == -1){
	 	perror("********** Error in getsockname() **********");
	 	exit(-1);
	 }

	proxy_server_port = proxy_server.sin_port;
	printf("admin: started proxy server on '%s' at '%hu'\n", proxy_server_host, ntohs(proxy_server_port));
		
	free(proxy_server_host);
	return proxy_socket;
}

int split_http_request(char** splitted_req,char *conn_req_buffer){
					    int index = 0;
					    int i;
					    int index_str = 0;
					    splitted_req[index] = (char*)calloc(SPLITED_PTR_SIZE,sizeof(char));
					    int request_len = strlen(conn_req_buffer); 
					    for ( i = 0; i < request_len; i++)
					    {
					    	if(conn_req_buffer[i] != '\r')
					    	{
					    		splitted_req[index][index_str] = conn_req_buffer[i];
					    		index_str++;
					    	}
					    	else
					    	{
					    		i++;
					    		index++;
					    		index_str = 0;
					    		splitted_req[index] = (char*)calloc(SPLITED_PTR_SIZE,sizeof(char));
					    	}
					    }
					    return index;

}

int getContentLength(char **splitted_response, int index_res){
			int i;
			
		  const  char *content_len_st= "Content-Length: ";
		    char *content_len;
		    int cont_len = 0;
		    for ( i = 0; i < index_res; i++)
		    {
		    	char *exist;
		    	exist = strstr(splitted_response[i],content_len_st);
		    	if(exist)
		    		{
		    			cont_len = strlen(splitted_response[i])-strlen(content_len_st);
		    			content_len = (char*)calloc(cont_len,sizeof(char));
		    			int k = 0;
		    			for(k=0;k<cont_len;k++){
		    				content_len[k] = splitted_response[i][k+strlen(content_len_st)];
		    			}
		    			break;
		    		}
		    		else{
		    		}

		    }
		    
			int content_len_int = 0;
			int len_creator = 0;
			int temp = cont_len;
			for(temp = cont_len - 1;temp>=0;temp--){
				char charcter = content_len[temp];
				content_len_int = content_len_int + atoi(&charcter)*(int)pow(10.0, len_creator);
				len_creator++;
			}
			return content_len_int;
}

int getPortNumber(char *host_name){
	    int port_number = 0;
	    int port_creater = 0;
	    int i;
	    for(i=strlen(host_name)-1;i>=0;i--){
	    	char charcter = host_name[i];
	    	if((charcter >= 'A' && charcter <= 'Z') || (charcter >= 'a' && charcter <= 'z'))
		    {
		        break;
		    }
		    if(charcter == ':'){
		    	host_name[i] ='\0';
		    	break;
		    }

	    	if(charcter >= '0' && charcter <= '9'){
	    			host_name[i] ='\0';
	    			port_number = port_number + atoi(&charcter)*(int)pow(10.0, port_creater);
	    		}
	    	charcter='\0';
	    	port_creater++;	
	    }

	    if(port_number == 0){
	    	port_number = 80;
	    }
	    return port_number;
}

int check_isCached(char *req){
	int i = 0;
	for(i = 0;i<cache_entry_count;i++){
		const char *yyy =cache[i].my_req[0].c_str();
		int isPresent = strcmp(req,yyy);
		if(isPresent == 0)
			return i;
	}
	return -1;
}

void *callback_method(void *sock_descriptor)
{
	int *ss;
    ss = (int *) sock_descriptor;
    int sock_desc = *ss;
	int cli_sock = 0;
	unsigned int len;
	struct sockaddr_in client;
	char *conn_req_buffer = (char*)calloc(HTTP_REQUEST_BUFF_SIZE,sizeof(char));
	len = sizeof(client);


	if((cli_sock = accept(sock_desc,(struct sockaddr*)&client,&len))==-1)
		{
			perror("********** Error in accept() ********** ");
			exit(-1);
		}

		char ipaddress_display[20];
		if(inet_ntop(AF_INET,&client.sin_addr.s_addr,ipaddress_display,sizeof(ipaddress_display))!=NULL){
			
			}
		else{
			perror("********** Error in inet_ntop() while getting IP addess ********** ");
			exit(-1);
		}

	   	struct timeval start_timer_count, end_timer_count;
		

	   if (read(cli_sock, conn_req_buffer,HTTP_REQUEST_BUFF_SIZE ) < 0) {
	     perror("********** Error in Read() ********** ");
	     exit(-1);
	    }
	   
	     int i = 0;
	   	char *splitted_req[15];


	   	int l =0;
	   	char original_req[250];
	   	for(l = 0;;l++){
	   		if(conn_req_buffer[l] == '\r')
	   		{
	   			original_req[l] = '\0';
	   			break;
	   		}
	   		else
	   		{		
	   			original_req[l] = conn_req_buffer[l];
	   		}	
	   	}
	   	original_req[l]='\0';

	   	char url[500];
	   	int  b = 0; int q =0;
	   	int check_now = 0;
	   	for(l = 0;;l++){
	   		if(conn_req_buffer[l] == ' ' && check_now == 1)
	   		{
	   			url[q] = '\0';
	   			break;

	   		}
	   		if(conn_req_buffer[l] == ' ' && check_now == 0)
	   		{
	   			check_now = 1;
	   		}
	   		if(check_now == 1){
	   			url[q] = conn_req_buffer[l];
	   			q++;
	   		}


	   	}

	   	int index = split_http_request(splitted_req,conn_req_buffer);

	   	for ( i = 0; i < index; i++)
					    {
					    }

					 const char *host_st= "Host: ";
					    char *host_name;
					    for ( i = 0; i < index; i++)
					    {
					    	char *exist;
					    	exist = strstr(splitted_req[i],host_st);
					    	if(exist)
					    		{
					    			int host_len = strlen(splitted_req[i])-strlen(host_st);
					    			host_name = (char*)calloc(host_len+1,sizeof(char));
					    			int k = 0;
					    			for(k=0;k<host_len;k++){
					    				host_name[k] = splitted_req[i][k+strlen(host_st)];
					    			}
					    			host_name[host_len] = '\0';
					    			break;
					    		}
					    		else{
					    		}

					    }
		
	    int port_number = getPortNumber(host_name);

	if(RUNTHIS){
	   
		char* new_request = (char*) calloc(strlen(splitted_req[0])+strlen(splitted_req[3])+9,sizeof(char));
		 sprintf(new_request, "%s\r\n%s\r\n\r\n", splitted_req[0], splitted_req[3]);
			int cache_index;
	    

	     if((cache_index = check_isCached(new_request)) != -1){
	    	gettimeofday (&start_timer_count, NULL);
	    	int count ;
			int o = 0;
			//cout << cache[cache_index].track_char.size() << "\n";
			while(o < cache[cache_index].my_resp.size()){
				//n = read(actual_host_socket, , REQ_BUFF_SIZE);
				int nnn;
	    		/*if(o == 0){
	    			nnn = 1000;
	    		}
	    		else{*/
	    			nnn = cache[cache_index].track_char[o];
	    		// /}
				const char* vr = cache[cache_index].my_resp[o].c_str();
				write(cli_sock,vr,nnn);
				dontcallme(cli_sock);
				usleep(100);
				o++;
			}

			gettimeofday (&end_timer_count, NULL);
			long sec1 = (end_timer_count.tv_sec - start_timer_count.tv_sec)*1000 + (end_timer_count.tv_usec - start_timer_count.tv_usec) * 0.001;
			printf("%s | %s | CACHE_HIT | %d | %lu", ipaddress_display,url,cache[cache_index].contentlength,sec1);
			void *ptr;
	    	return ptr;
	    }

	    char response[REQ_BUFF_SIZE];
		bzero(response, REQ_BUFF_SIZE);
	    
	    int actual_host_socket = socket(AF_INET, SOCK_STREAM, 0);
	    if(actual_host_socket == -1){
	    	perror("socket()");
	    	exit(-1);
	    }

		struct sockaddr_in sockaddr_host;
		struct hostent* server_hostent;

		
		server_hostent = gethostbyname(host_name);
		if (server_hostent == NULL) {
			 perror("********** Error in gethostbyname() ********** ");
			exit(-1);
		}
		free(host_name); 
		bzero((char*) &sockaddr_host, sizeof(sockaddr_host));
		
		bcopy((char *)server_hostent->h_addr, (char *)&sockaddr_host.sin_addr.s_addr, server_hostent->h_length);
		
		sockaddr_host.sin_family = AF_INET;
		
		sockaddr_host.sin_port = htons(port_number);

		pthread_mutex_lock(&thread_lock);
		if (connect(actual_host_socket, (struct sockaddr*) &sockaddr_host, sizeof(sockaddr_host)) < 0) {
			printf("Error Connecting To server...\n");
			exit(-1);
		}
		gettimeofday (&start_timer_count, NULL);

		if (write(actual_host_socket, new_request, strlen(new_request)) < 0) {
			printf("Error with sending request to server...");
			exit(0);
		}
		memset(response, '\0', REQ_BUFF_SIZE);
		int n = read(actual_host_socket, response, 1000);
					char *splitted_response[15];
				    int index_res = 0;
				    int index_str_res = 0;
				    
				    char *end_res_header = strstr(response, "\r\n\r\n");
			    	int endstr_index = end_res_header - response;

			    	
			    	splitted_response[index_res] = (char*)calloc(SPLITED_PTR_SIZE,sizeof(char));

				    for ( i = 0; i < endstr_index; i++)
				    {
				    	if(response[i] !='\r')
				    	{
				    		splitted_response[index_res][index_str_res] = response[i];
				    		index_str_res++;
				    	}
				    	else
				    	{
				    		i++;
				    		index_res++;
				    		index_str_res = 0;
				    		splitted_response[index_res] = (char*)calloc(SPLITED_PTR_SIZE,sizeof(char));
				    	}
				    }
				    
				    


				    for ( i = 0; i < index_res; i++)
				    {
				    }

				    int ccccc  = getContentLength(splitted_response,index_res);
		int strlen_new_request = strlen(new_request);
				
			
			cache[cache_entry_count].my_req.push_back(new_request);
			cache[cache_entry_count].contentlength = ccccc;
				int use_size;
				if(ccccc < 10000)
					use_size = 1000;
				else
					use_size = REQ_BUFF_SIZE;
				long sec;
			while(n > 0){
				
				write(cli_sock,response,n);
				if(ccccc < 10000)
					dontcallme(actual_host_socket);
				cache[cache_entry_count].my_resp.push_back(response);
				cache[cache_entry_count].track_char.push_back(n);
				bzero(response, REQ_BUFF_SIZE);
				if(ccccc <  10000)
					usleep(1000);
				n = read(actual_host_socket, response, use_size);
				
			  }
			close(actual_host_socket);

			gettimeofday (&end_timer_count, NULL);
			sec = (end_timer_count.tv_sec - start_timer_count.tv_sec)*1000 + (end_timer_count.tv_usec - start_timer_count.tv_usec) * 0.001;
			printf("%s | %s | CACHE_MISS | %d | %lu", ipaddress_display,url,cache[cache_entry_count].contentlength,sec);
			cache_entry_count++;
			pthread_mutex_unlock(&thread_lock);
}
return 0;
}

void
usage(const std::string& program_name)
{
	std::cout << program_name << " ./proxy " << std::endl;
}

int main(){	

	if (pthread_mutex_init(&thread_lock, NULL) != 0)
    {
        printf("pthread_mutex_init failed ********************\n");
        return 1;
    }
		int socket_desc = start_proxy_server();
		while(1){			
		    pthread_t thread;
		    int thread_return;
		    int *ptr_socket_desc = &socket_desc;
		    thread_return = pthread_create(&thread, NULL, callback_method, (void*) ptr_socket_desc);
		    pthread_join( thread, NULL);
		    printf("\n");
		}
	return 0;
}
int dontcallme(int sock_descriptor)
{
    int toggel;
    #if defined(O_NONBLOCK)
    toggel = fcntl(sock_descriptor, F_GETFL, 0);    
    if (-1 == (toggel = fcntl(sock_descriptor, F_GETFL, 0)))
        toggel = 0;
    int return_fcntl = fcntl(sock_descriptor, F_SETFL, toggel | O_NONBLOCK);
    return return_fcntl;
	#else
    toggel = 1;
    int ioctl_return = ioctl(sock_descriptor, FIOBIO, &toggel);
    return ioctl_return;
#endif
}