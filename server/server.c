#include "head/c_05.h"
#define O_FLAG (O_RDWR|O_CREAT|O_TRUNC)
#define FILENAME_ "temp/ab_01.c"
#define FILENAME_O "temp/ab_01"
#define COMP_FILE "temp/ab_compMsg.txt"
#define JUDG_IN "data/a.in"
#define JUDG_OUT "temp/ab_a.out"
#define JUDG_ANS "data/a.ans"
#define JUDG_NUM 5/*1 <= n <= 9*/

void receive_(int connfd, char *filename_);
int compile_(int connfd, char *comp_file, char *filename_, char *filename_o);
void judge_(int connfd, char *judg_out_, char *filename_o);

void mov_file(int send_fd, int recive_fd);
void mov_msg(char *msg, int fd);

void *server_thread(void *var);

static int cnt = 0;
int push_();
void pop_();

int main(int argc, char **argv)
{
	int listenfd, *Pconnfd;
	char *port;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	
	pthread_t tid;
	
	while(1)
	{
		char ch = getopt(argc, argv, "p:");
		if(ch == -1)break;
		switch(ch)
		{
			case 'p':
				port = optarg;
			break;
			
			default:
				fprintf(stderr, "Wrong Input: %c\n", (char)optopt);
			break;
		}
	}
	listenfd = Open_listenfd(port);
	
	while(1)
	{
		clientlen = sizeof (struct sockaddr_storage);
		Pconnfd = (int*)malloc(sizeof(int));
		*Pconnfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);
		/*connect success*/
		fprintf(stdout, "[%d]: connect success!\n", *Pconnfd);

		pthread_create(&tid, NULL, server_thread, Pconnfd);
	}
	
	exit(0);
}

void *server_thread(void *var)
{
	int connfd = *((int*)var);
	Pthread_detach(pthread_self());
	free(var);

	int a = push();
	int b = a % 10;
	a /= 10;
	
	char filename_[] = FILENAME_;
	char filename_o[] = FILENAME_O;
	char comp_file[] = COMP_FILE;
	char judg_out[] = JUDG_OUT;

	filename_[5] = filename_o[5] = comp_file[5] = judg_out[5] = a + '0';
	filename_[6] = filename_o[6] = comp_file[6] = judg_out[6] = b + '0';
	
	receive_(connfd, filename_);
	
	int flg = compile_(connfd, comp_file, filename_, filename_o);
	if(flg != -1)
		judge_(connfd, judg_out, filename_o);
	
	mov_msg("@", connfd);
	fprintf(stdout, "\n");
	
	Close(connfd);
	
	return NULL;
}

void receive_(int connfd, char *filename_)
{
	fprintf(stdout, "[%d]: receiving...\n", connfd);
	
	int locfd = open(filename_, O_FLAG);
	if(locfd < 0)
		unix_error("Open file error");
	
	mov_file(connfd, locfd);
	Close(locfd);
	
	fprintf(stdout, "[%d]: received success!\n", connfd);
	return;
}

int compile_(int connfd, char *comp_file, char *filename_, char *filename_o)
{
	fprintf(stdout, "[%d]: compiling...\n", connfd);
	mov_msg("compiling...\n", connfd);
	
	int flg = -1;

	int fd = open(comp_file, O_FLAG);
	if(fd < 0)
		unix_error("Open file error");
	
	int sfd = Dup(STDERR_FILENO);
	Dup2(fd, STDERR_FILENO);
	
	if(Fork() == 0)
	{
		char *argv1[] = {"/bin/gcc", filename_, "-o", filename_o, "-std=c99", NULL};
		char *envp[] = {"PATH=/bin", NULL};
		Execve(argv1[0], argv1, envp);
	}
	Wait(NULL);
	Dup2(sfd, STDERR_FILENO);/*recover*/
	Close(sfd);
	Close(fd);
	
	fd = open(comp_file, O_RDONLY);
	if(fd < 0)
		unix_error("Open file error");
	
	struct stat st;
	Fstat(fd, &st);
	if(st.st_size == 0)
	{
		fprintf(stdout, "[%d]: compile success!\n", connfd);
		mov_msg("compile success!\n", connfd);
		flg = 1;
	}
	else
	{
		fprintf(stdout, "[%d]: compile fail!\n", connfd);
		mov_msg("compile fail!\n", connfd);
		mov_file(fd, connfd);
	}
	
	Close(fd);
	Remove(filename_);
	Remove(comp_file);
	return flg;
}

void judge_(int connfd, char *judg_out, char *filename_o)
{
	fprintf(stdout, "[%d]: running...\n", connfd);
	mov_msg("running...\n", connfd);
	
	int flg = -1;
	
	char judg_in[] = JUDG_IN;
	char judg_ans[] = JUDG_ANS;
	
	int i;
	for(i = 1; i <= JUDG_NUM; i++)
	{
		judg_in[5] = i + '0';
		judg_out[8] = i + '0';
		judg_ans[5] = i + '0';
		
		int in_fd = open(judg_in, O_RDONLY);
		int out_fd = open(judg_out, O_FLAG);
		
		if(in_fd < 0 || out_fd < 0)
			unix_error("Open file error");
		
		int sin_fd = Dup(STDIN_FILENO);
		Dup2(in_fd, STDIN_FILENO);
		int sout_fd = Dup(STDOUT_FILENO);
		Dup2(out_fd, STDOUT_FILENO);
		
		if(Fork() == 0)
		{
			char *argv[]={filename_o, NULL};
			char *envp[]={0, NULL};
			Execve(argv[0], argv, envp);
		}
		Wait(NULL);
		
		Dup2(sin_fd, STDIN_FILENO);
		Close(sin_fd);
		Dup2(sout_fd, STDOUT_FILENO);
		Close(sout_fd);
		Close(in_fd);
		Close(out_fd);
		
		out_fd = open(judg_out, O_RDONLY);
		int ans_fd = open(judg_ans, O_RDONLY);
		if(out_fd < 0 || ans_fd < 0)
			unix_error("Open file error");
		
		while(1)
		{
			char out_c, ans_c;
			int out_n, ans_n;
			do{
				out_n = Rio_readn(out_fd, &out_c, 1);
				if(out_n == 0)break;
			}while(out_c == ' ' || out_c == '\n');
			
			do{
				ans_n = Rio_readn(ans_fd, &ans_c, 1);
				if(ans_n == 0)break;
			}while(ans_c == ' ' || ans_c == '\n');
			
			if(out_n == 0 || ans_n == 0)
			{
				if(out_n != ans_n)
					flg = i;
				break;
			}
			
			if(out_c != ans_c)
			{
				flg = i;
				break;
			}
		}
		
		Close(out_fd);
		Close(ans_fd);
		Remove(judg_out);

		if(flg > 0)
			break;
	}
	

	if(flg < 0)
	{
		fprintf(stdout, "[%d]: Absolutly correct!\n", connfd);
		mov_msg("Absolutly correct!\n", connfd);
	}	
	else
	{
		fprintf(stdout, "[%d]: Wrong answer on test %d!\n", connfd, flg);
		mov_msg("Wrong answer on test %d!\n", connfd);
	}	
	
	Remove(filename_o);
	return;
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

int push()
{
	cnt++;
	if(cnt > 99)
		cnt = 1;

	return cnt;
}
