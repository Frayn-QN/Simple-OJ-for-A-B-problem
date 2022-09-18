#include "head/c_05.h"
#define TT printf("1\n");

void mov_file(int send_fd, int recive_fd);
void mov_msg(char *msg, int fd);

int main(int argc, char *argv[])
{
	int clientfd;
	char *filename, *host, *port;
	
	while(1)
	{
		char ch = getopt(argc, argv, "t:s:p:");
		if(ch == -1)break;
		switch(ch)
		{
		case 't':
			filename = optarg;
			break;
			
		case 's':
			host = optarg;
			break;
			
		case 'p':
			port = optarg;
			break;
			
		default:
			fprintf(stderr, "Wrong Input: %c\n", (char)optopt);
			break;
		}
	}
	
	clientfd = Open_clientfd(host, port);

	fprintf(stdout, "summitting...\n");
	int locfd = open(filename, O_RDONLY);
	if(locfd < 0)
		unix_error("Open file error");
	mov_file(locfd, clientfd);
	mov_msg("@", clientfd);
	Close(locfd);
	fprintf(stdout, "summit success!\n");
	
	mov_file(clientfd, STDOUT_FILENO);

	Close(clientfd);
	exit(0);
}

void mov_file(int send_fd, int recive_fd)
{
	size_t n;
	char buf;
	
	while(1)
	{
		n = Rio_readn(send_fd, &buf, 1);
		if(n == 0 || buf == '@')break;
		Rio_writen(recive_fd, &buf, 1);
	}
}

void mov_msg(char *msg, int fd)
{
	Rio_writen(fd, msg, strlen(msg));
}
