#define RIO_BUFSIZE (8192)
typedef struct
{
	int rio_fd;//与缓冲区绑定的文件描述符的编号
	int rio_cnt;//缓冲区中还未读取的字节数
	char *rio_bufptr; //当前下一个未读取字符的地址
	char rio_buf[RIO_BUFSIZE];
}rio_t;

ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

void rio_readinitb(rio_t *rp,int fd);
static ssize_t rio_read(rio_t *rp, char*usrbuf, size_t n);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);


ssize_t rio_readn(int fd, void *usrbuf, size_t n)//无缓冲输入
{
	size_t nleft = n;
	ssize_t nread;
	char *bufp = (char*)usrbuf;
	
	while(nleft > 0)
	{
		if((nread = read(fd, bufp, nleft)) < 0)
		{
			if(errno == EINTR)
				nread = 0;
			else
				return -1;
		}
		else if(nread == 0)
			break;/*EOF*/
		
		nleft -= nread;
		bufp += nread;
	}
	
	return (n - nleft);
}

ssize_t rio_writen(int fd, void *usrbuf, size_t n)//无缓冲输出
{
	size_t nleft = n;
	ssize_t nwritten;
	char *bufp = (char*)usrbuf;
	
	while(nleft > 0)
	{
		if((nwritten = write(fd, bufp, nleft)) <= 0)
		{
			if(errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		
		nleft -= nwritten;
		bufp += nwritten;
	}
	
	return n;
}

void rio_readinitb(rio_t *rp,int fd)//rio_t预处理
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
	
	return;
}

static ssize_t rio_read(rio_t *rp, char*usrbuf, size_t n)//底层读取函数
{
	int cnt;
	
	while(rp->rio_cnt <= 0)
	{
		rp->rio_cnt= read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		
		if(rp->rio_cnt < 0)
		{
			if(errno != EINTR)
				return -1;
		}
		else if(rp->rio_cnt == 0)
			return 0;/*EOF*/
		else
			rp->rio_bufptr = rp->rio_buf;/*reset ptr*/
	}
	
	cnt = n;
	if(rp->rio_cnt < n)
		cnt = rp->rio_cnt;
	/*cnt=min(n,rp->rio_cnt)*/
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	
	return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)//带缓冲读取一行
{
	int n, rc;
	char c, *bufp = usrbuf;
	
	for(n=1; n < maxlen; n++)//已接收字符的数量
	{
		if((rc=rio_read(rp, &c, 1)) == 1)
		{
			*bufp++ = c;
			if(c == '\n')
			{
				n++;
				break;
			}
		}
		else if(rc == 0)
		{
			if(n == 1)//如果第一次循环就没接收到数据，则代表无数据可接收
				return 0;
			else
				break;
		}
		else                    
			return -1;
	}
	*bufp = 0;
	
	return (n - 1);
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)//带缓冲读
{
	size_t leftcnt = n;
	ssize_t nread;
	char *buf = (char *)usrbuf;
	
	while(leftcnt > 0)
	{
		if((nread = rio_read(rp, buf, n)) < 0)
			return -1;
		else if(nread == 0)
			break;/*EOF*/
		
		leftcnt -= nread;
		buf += nread;
	}
	
	return (n - leftcnt);
}
