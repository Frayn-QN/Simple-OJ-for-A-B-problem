#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>//线程

#include "rio.h"
#include "socket.h"
#include "error_deal.h"

#define MAXLINE (1024)
