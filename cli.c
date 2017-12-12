#include "unp.h"
#include <stdio.h>

#define KNRM "\x1B[0m"
#define KGRN "\x1B[32m"
const char *grnbuf = KGRN;
const char *nrmbuf = KNRM;
void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;

	stdineof = 0;
	FD_ZERO(&rset);
	for( ;  ;)
	{
		if(stdineof ==0)
		{
			FD_SET(fileno(fp), &rset);
		}
		FD_SET (sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset))
		{
			if((n=Read(sockfd, buf, MAXLINE))==0)
			{
				if(stdineof ==1 )
					return;
				else 
					err_quit("str_cli: server teminated prematurely");
			}
			Write(fileno(stdout), grnbuf, strlen(grnbuf));
			Write(fileno(stdout), buf, n);
			Write(fileno(stdout), nrmbuf , strlen(nrmbuf));

		}

		if(FD_ISSET(fileno(fp), &rset))
		{
			if((n= Read(fileno(fp), buf, MAXLINE))==0)
			{
				stdineof =1 ;
				Shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			Write(sockfd, buf, n);
		}

	}
}
int 
main(int argc, char* argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2)
	{
		err_quit("usage: tcpcli <IPaddress>");
	}
	sockfd  = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family  = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	str_cli(stdin, sockfd);

	exit(0);
}
