/* include fig01 */
#include	"unp.h"
const char *pwd = "toor";
const char *reenter = "re-enter your passwd\n";
struct name{
	char name[20];
};
int
main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[FD_SETSIZE];
	int 				flag[FD_SETSIZE]; 
	struct name				cliname[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++){
		client[i] = -1;			/* -1 indicates available entry */
		flag[i]   = 0;
	}
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
/* end fig01 */

/* include fig02 */
	for ( ; ; ) {
		rset = allset;		/* structure assignment */
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
#ifdef	NOTDEF
			printf("new client: %s, port %d\n",
					Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, strlen(cliaddr.sin_addr)),
					ntohs(cliaddr.sin_port));
#endif
			char str[20];
			Inet_ntop(AF_INET, &cliaddr.sin_addr, str,20);
			printf("new client: %s, port %d\n",
					str,
					ntohs(cliaddr.sin_port));

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;	/* save descriptor */
					break;
				}
			if (i == FD_SETSIZE)
				err_quit("too many clients");

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				memset(buf, '\0',MAXLINE);
				if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
						/*4connection closed by client */
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
					flag  [i] = 0;
					printf("client %s left\n", cliname[i].name);
					*cliname[i].name = "\0";
				}
				else
				{
					
					if(flag[i] == 0){
	//					cliname[i].name = buf;
						snprintf(cliname[i].name, strlen(buf), "%s", buf);
						flag[i]         = 1;
					}
					else if(flag[i] ==1){
						if( buf[0]!= pwd[0]
								|| buf[1]!=pwd[1]
								|| buf[2]!=pwd[2]
								|| buf[3]!=pwd[3])
							Writen(sockfd,reenter,strlen(reenter));
						else
						{
							flag[i] = 2;
						}
					}
					
					else {
						int tempfd;
	//					printf("responce to: %s\n",cliname[i].name);
						char temp[128];
						snprintf(temp, 
								n + 7 + sizeof(cliname[i].name),
								"%s says: %s", cliname[i].name, buf);
						for(int k = 0; k <= maxi; ++ k)
						{
							if(k == i){
							//	char t_b[30];
							//	snprintf(t_b,30,"%s","    responce from server\n");
								//Writen(sockfd, t_b, 30);
							}
							else {
								if((tempfd = client[k]) < 0)
									continue;
								//printf("%d\n",strlen (buf));
								Writen(tempfd, temp, strlen(temp));
							}
						}
				}
			}

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
}
/* end fig02 */
