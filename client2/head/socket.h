#define LISTENQ 1024
typedef struct sockaddr SA;

int open_clientfd(char *hostname,char *port);
int open_listenfd(char *port);

int open_clientfd(char *hostname,char *port)//client
{
	int clientfd;
	struct addrinfo hints, *listp, *p;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_flags |= AI_ADDRCONFIG;
	getaddrinfo(hostname, port, &hints, &listp);
	
	for(p=listp; p; p = p->ai_next)
	{
		if((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			continue;/*socket failed*/
		
		if(connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
			break;/*success*/
	}
	
	freeaddrinfo(listp);
	if(!p)
		return -1;
	else
		return clientfd;
}

int open_listenfd(char *port)//server
{
	struct addrinfo hints, *listp, *p;
	int listenfd, optval=1;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	hints.ai_flags |= AI_NUMERICSERV;
	getaddrinfo(NULL, port, &hints, &listp);
	
	for(p = listp; p; p = p->ai_next)
	{
		if((listenfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
			continue;/*socket failed*/
		
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
		//zhan xian
		
		if(bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break;/*success*/
		
		close(listenfd);/*bind failed*/
	}
	
	freeaddrinfo(listp);
	if(!p)
		return -1;
	
	if(listen(listenfd, LISTENQ) < 0)
	{
		close(listenfd);
		return -1;
	}
	
	return listenfd;
}
