void unix_error(char *msg);
void posix_error(int code, char *msg);
void gai_error(int code, char *msg);
void app_error(char *msg);


ssize_t Rio_readn(int fd, void *usrbuf, size_t n);
ssize_t Rio_writen(int fd, void *usrbuf, size_t n);
ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n);

//socket
int Open_clientfd(char *hostname,char *port);
int Open_listenfd(char *port);
int Accept(int listenfd, struct sockaddr *addr, int *addrlen);
int Getaddrinfo(const char *host, const char *service, const struct addrinfo *hints, struct addrinfo **result);

//file
//solo open
int Close(int fd);
int Remove(const char *filename);
int Dup(int oldfd);
int Dup2(int oldfd, int newfd);
int Fstat(int fd, struct stat *buf);

pid_t Fork();
pid_t Wait(int *statusp);
int Execve(const char *filename, char *const argv[], char *const envp[]);

//pthread
int Pthread_detach(pthread_t tid);


void unix_error(char* msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

void posix_error(int code, char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(code));
	exit(0);
}

void gai_error(int code, char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, gai_strerror(code));
	exit(0);
}

void app_error(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(0);
}

ssize_t Rio_readn(int fd, void *usrbuf, size_t n)
{
	ssize_t flg = rio_readn(fd, usrbuf, n);
	if(flg < 0)
		unix_error("Rio_readn error");
	
	return flg;
}

ssize_t Rio_writen(int fd, void *usrbuf, size_t n)
{
	ssize_t flg = rio_writen(fd, usrbuf, n);
	if(flg < 0)
		unix_error("Rio_writen error");
	
	return flg;
}

ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
	ssize_t flg = rio_readlineb(rp, usrbuf, maxlen);
	if(flg < 0)
		unix_error("Rio_readlineb error");
	
	return flg;
}

ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
	ssize_t flg = rio_readnb(rp, usrbuf, n);
	if(flg < 0)
		unix_error("Rio_readnb error");
	
	return flg;
}


int Open_clientfd(char *hostname,char *port)
{
	int flg = open_clientfd(hostname, port);
	if(flg < 0)
		unix_error("Open_clientfd error");
	
	return flg;
}

int Open_listenfd(char *port)
{
	int flg = open_listenfd(port);
	if(flg < 0)
		unix_error("Open_listenfd error");
	
	return flg;
}

int Accept(int listenfd, struct sockaddr *addr, int *addrlen)
{
	int flg = accept(listenfd, addr, addrlen);
	if(flg < 0)
		unix_error("Accept error");
	
	return flg;
}

int Getaddrinfo(const char *host, const char *service, const struct addrinfo *hints, struct addrinfo **result)
{
	int flg = getaddrinfo(host, service, hints, result);
	if(flg != 0)
		gai_error(flg, "Getaddrinfo error");
	
	return flg;
}

int Close(int fd)
{
	int flg = close(fd);
	if(flg < 0)
		unix_error("Close file error");
	
	return flg;
}

int Remove(const char *filename)
{
	int flg = remove(filename);
	if(flg < 0)
		unix_error("Remove file error");
	
	return flg;
}

int Dup(int oldfd)
{
	int fd = dup(oldfd);
	if(fd < 0)
		unix_error("Dup error");
	
	return fd;
}

int Dup2(int oldfd, int newfd)
{
	int fd = dup2(oldfd, newfd);
	if(fd < 0)
		unix_error("Dup2 error");
	
	return fd;
}

int Fstat(int fd, struct stat *buf)
{
	int flg = fstat(fd, buf);
	if(flg < 0)
		unix_error("Fstat error");
	
	return flg;
}


pid_t Fork()
{
	pid_t pid = fork();
	if(pid < 0)
		unix_error("Fork error");
	
	return pid;
}

pid_t Wait(int *statusp)
{
	pid_t pid = wait(statusp);
	if(pid < 0)
		unix_error("Fork error");
	
	return pid;
}

int Execve(const char *filename, char *const argv[], char *const envp[])
{
	int flg = 1;
	flg = execve(filename, argv, envp);
	if(flg < 0)
		unix_error("Execve error");
	
	return flg;
}

int Pthread_detach(pthread_t tid)
{
	int flg = pthread_detach(tid);
	if(flg != 0)
		posix_error(flg, "Pthread_detach error");
	
	return flg;
}
