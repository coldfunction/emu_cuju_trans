#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <err.h>
#include <errno.h>


#define TOTAL_LEN (64*1024)

long long get_data(FILE *fp){

    char *line = NULL;
    size_t tmplen = 0;
    ssize_t read;
    read = getline(&line, &tmplen, fp);
    if (read == -1)
        return -1;

    long long b_len;
    sscanf(line, "%lld", &b_len);

    return b_len;
}


int main(int argc , char *argv[])

{
    //socket build
//    char inputBuffer[256] = {};
    long long TOTAL_DATA_SIZE = 1024*1024*1024;
	char *inputBuffer;
	int len = TOTAL_LEN;
//	inputBuffer = malloc(len);
	inputBuffer = malloc(TOTAL_DATA_SIZE);

    char message[] = {"Hi,this is server okokokok.\n"};
    int sockfd = 0,forClientSockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    FILE *fp;
    fp = fopen("runtime_latency_trans_rate0.txt", "r");
//    fp = fopen("test.txt", "r");
   char *line = NULL;

    //socket connect
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo,sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = inet_addr("172.31.3.2");
    serverInfo.sin_port = htons(8700);
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(sockfd,5);

        forClientSockfd = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
//        forClientSockfd = accept4(sockfd,(struct sockaddr*) &clientInfo, &addrlen, SOCK_NONBLOCK);
//    int get_next = 1;
    while(1){


        long long b_len;
        if((b_len = get_data(fp)) == -1)
            break;

  //      long long run_time;
 //       if(run_time = get_data(fp) == -1)
//            break;

        TOTAL_DATA_SIZE = b_len;





	    long long total = TOTAL_DATA_SIZE;

        printf("@@ total len = %lld\n", total);
        if(total == 0) {
            printf("@@ 0 reset\n");
            continue;
        }
        if(total < TOTAL_LEN) {
            len = total;
        }


		long offset = 0;
        int ret = 0;
		while (1)  {
            int i = 1;
            setsockopt(forClientSockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));

            ret = recv(forClientSockfd,inputBuffer+offset,len,0) ;
            if(ret < 0) {
                if(errno == EINTR)
                    continue;
            }
//            printf("receive %d bytes, offset = %d\n", ret, offset);
 //           if(inputBuffer[offset] == '@')
  //              printf("okok\n");
 //           printf("errno = %d\n", errno);
			offset += ret;
            total -= ret;
            if(total <= 0) break;
            if(offset+len >= TOTAL_DATA_SIZE) break;

//			printf("cocotion recv len = %d\n", ret);
			/*if(len == 0) {
				total-=offset;
				printf("cocotion test rest recv = %d\n", total);
				if(total == 0)
					break;
				else {
					offset = 0;
					len = TOTAL_LEN;
				}
			}*/

		}
		while (total > 0)  {
            int i = 1;
            setsockopt(forClientSockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&i, sizeof(i));

            ret = recv(forClientSockfd,inputBuffer+offset,total,0)  ;
            if(ret < 0) {
                if(errno == EINTR)
                    continue;
            }
  //          printf("receive %d bytes, offset = %d, total = %d\n", ret, offset, total);
            offset+=ret;
            total-=ret;

        }


		printf("cocotion test ok I recv all\n");
		int i;
//        for(i = 0; i < TOTAL_DATA_SIZE; i++)
 //           printf("%c ", inputBuffer[i]);
		//for(i = 0; i < TOTAL_LEN; i++) {
		//	if(inputBuffer[i] != '@' ) break;
		//}
		//if(i != TOTAL_LEN) {
		//	printf("i = %d\n", i);
		//	message[0] = '@';
		//}
        send(forClientSockfd,message,sizeof(message),0);
//        printf("Get:%s\n",inputBuffer);
    }

    if(line)
        free(line);
       fclose(fp);

    return 0;
}
