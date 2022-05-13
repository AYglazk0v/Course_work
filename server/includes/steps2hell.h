#ifndef STEPS2HELL_H
# define STEPS2HELL_H

# ifndef MAXBUF
#  define MAXBUF 4096
# endif

# define PORT 3389

# define RESET	"\033[0m"
# define RED	"\033[1;31m"
# define YELLOW	"\033[1;33m"
# define GREEN	"\033[1;32m"

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>
# include <dirent.h>

# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/wait.h>
# include <arpa/inet.h>
# include <openssl/ssl.h>
# include <openssl/err.h>
# include <ifaddrs.h>
# include <netdb.h>

# include "utils.h"

typedef struct s_info
{
	char				*s_dir;
	char				buff[MAXBUF];
	int					fd_serv, fd_client, fd;
	socklen_t			sock_len;
	SSL_CTX				*ctx;
	SSL					*ssl;
	struct	sockaddr_in	server_addr, client_addr;
}	t_info;

void	fn_checkStartDir(t_info *inf);
void	fn_connetion(t_info *inf);

#endif