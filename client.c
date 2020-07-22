#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>

#define TOTAL_LEN (64*1024)
//#define TOTAL_DATA_SIZE (4*1024*1024)

#define TIMEVAL_TO_DOUBLE(tv)   ((tv).tv_sec + \
›   ›                                   ((double)(tv).tv_usec) / 1000000)
#define TIMEVAL_TO_US(tv)   ((tv).tv_sec * 1000000 + (tv).tv_usec)

typedef struct timeval qemu_timeval;
#define qemu_gettimeofday(tp) gettimeofday(tp, NULL)

static inline double time_in_double(void)
{

   struct timespec ts;
   double ret;
   clock_gettime(CLOCK_MONOTONIC, &ts);
   ret = ts.tv_sec + ((double)ts.tv_nsec) / 1e9L;
   return ret;

}

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
    int sockfd = 0;
//    sockfd = socket(AF_INET , SOCK_STREAM | SOCK_NONBLOCK , 0);
    sockfd = socket(AF_INET , SOCK_STREAM, 0);
 //   fcntl(sockfd, F_SETFL, O_NONBLOCK);
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL,0) | O_NONBLOCK);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    long long TOTAL_DATA_SIZE = 1024*1024*1024;

    //socket connect

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
//    info.sin_addr.s_addr = inet_addr("172.31.3.2");
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8700);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
//    if(err==-1){
 //       printf("Connection error");
  //  }
  //

    if(err == 0) {
        printf("Connection okokok");
    }
    else if(err < 0 && errno == EINPROGRESS)          //errno == EINPROGRESS表示正在建立链接
    {
     // 等待连接完成，errno == EINPROGRESS表示正在建立链接
     fd_set set;
     FD_ZERO(&set);
     FD_SET(sockfd,&set);  //相反的是FD_CLR(_sock_fd,&set)

     time_t timeout = 10;          //(超时时间设置为10毫秒)
     struct timeval timeo;
     timeo.tv_sec = timeout / 1000;
     timeo.tv_usec = (timeout % 1000) * 1000;


/*
     int retval = select(sockfd + 1, NULL, &set, NULL, &timeo);           //事件监听
     if(retval < 0)
     {
            //建立链接错误close(_socket_fd)
        printf("select, but connection error\n");
     }
     else if(retval == 0) // 超时
     {
            //超时链接没有建立close(_socket_fd)
        printf("select, but connection error - timeout\n");
     }
*/
     //将检测到_socket_fd读事件或写时间，并不能说明connect成功
     if(FD_ISSET(sockfd,&set))
     {
           int error = 0;
           socklen_t len = sizeof(error);
           if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
           {
                  //建立简介失败close(_socket_fd)
                printf("isset, but connection error\n");
           }
           if(error != 0) // 失败
           {
                  //建立链接失败close(_socket_fd)
                printf("isset, but connection error\n");
           }
           else
            {
                  //建立链接成功
                printf("isset, connection okokok\n");
            }
     }
} else{
    printf("something error\n");
}


//    printf("connection okok\n");
    //Send a message to server
	//

        FILE *fp;
    	fp = fopen("runtime_latency_trans_rate0.txt", "r");
//    	fp = fopen("test.txt", "r");
/*            char *line = NULL;
            ssize_t read;
            size_t tmplen = 0;
            while(read = getline(&line, &tmplen, fp) != -1) {
                printf("%s\n", line);
            }
        if(line)
            free(line);
       fclose(fp);
*/

	char *buf;
	buf = malloc(TOTAL_DATA_SIZE);
	int i;
	for(i = 0; i < TOTAL_DATA_SIZE; i++)
		buf[i] = '@';


//    char message[] = {"Hi there"};

   int trans_rate = 1000;
   long long total = 0;
   long long less = 0;
   long long exceed = 0;
   while(1) {

    //printf("start send1@@~\n");
    char receiveMessage[100] = {};
	long long offset = 0;
	int len = TOTAL_LEN;

    //double start = time_in_double();

    long long b_len;
        
    if((b_len = get_data(fp)) == -1)
        break;
  
        //printf("QQ %lld\n", b_len);
    TOTAL_DATA_SIZE = b_len;
    long long total_len = TOTAL_DATA_SIZE;
    //printf("@@ total len = %lld\n", total_len);
    if(total_len == 0) continue;
    printf("%lld ", total_len);


    if(total_len < TOTAL_LEN) {
        len = total_len;
    }

    int predic_time = 0;
    predic_time = total_len/trans_rate;


    //total_len = b_len;
//	for(i = 0; i < TOTAL_DATA_SIZE/TOTAL_LEN; i++) {
    usleep(8000);
    double start = time_in_double();
    total++;
    while(1) {
        receiveMessage[0] = '@';
		//do {
            //int *buffer;
            //char *line = NULL;
            //ssize_t read;
            //size_t len = 0;
            //if(read = getline(&line, &len, fp) != -1) {
             //   printf("%s\n", line);
            //}


	    	int ret = send(sockfd,buf + offset, len,0);
   //         printf("ret = %d, offset = %lld, errno = %d\n", ret, offset, errno);

            if(ret < 0) {
                if(errno == EAGAIN) continue;
            }

                offset+=ret;
                total_len-=ret;
                if(offset+len >= TOTAL_DATA_SIZE)  break;
        //    printf("ret = %d\n", ret);
         //   printf("EAGAIN =  %d\n", EAGAIN);

	    	//len = len - ret;
			//printf("send len = %d\n", ret);
			//printf("rest len = %d\n", len);

		//}while (len);
		//printf("i = %d times\n", i);
		//len = TOTAL_LEN;
		//offset = 0;
	}
    len = TOTAL_DATA_SIZE-offset;
 //   printf(" offset = %d, len = %d\n", offset, len);
	while (1) {
        int ret = 0;
        ret = send(sockfd,buf + offset, len,0);
        //printf("ret = %d, offset = %d, len = %d\n", ret, offset, len);
        if(ret < 0) {
            if(errno == EAGAIN) continue;
        }
        len-=ret;
        offset+=ret;
        if(len <= 0) break;
    }
    int opts = fcntl(sockfd,F_GETFL);
    fcntl(sockfd, F_SETFL, opts &= ~O_NONBLOCK);

    recv(sockfd,receiveMessage,sizeof(receiveMessage),0);
/*
int ret = 0 ;
    offset =0;
    int llen = 28;
    while (1) {

        //ret = recv(sockfd,receiveMessage+offset,sizeof(receiveMessage),0);
        ret = recv(sockfd,receiveMessage+offset,llen,0);
        if(ret < 0) {
            if(errno == EAGAIN) continue;
        }
        offset += ret;
        llen-=ret;
        if(llen == 0) break;
    }*/
//

    double end = time_in_double();
    int timer = (int)((end-start) * 1000000);

//    printf("%s",receiveMessage);
//    printf("close Socket\n");
 //   printf("timer = %d us\n",timer);

//	printf("average trans rate = %lld\n", TOTAL_DATA_SIZE/timer);
    printf("%d\n",timer);
    if(timer > predic_time+1000) {
        printf("too long p: %d r: %lld\n", trans_rate, TOTAL_DATA_SIZE/timer);
        exceed++;
    }
    else if(timer < predic_time-1000) {
        printf("too short p: %d r: %lld\n", trans_rate, TOTAL_DATA_SIZE/timer);
        less++;
    }
    else {

    }

    printf("p: %d r: %lld\n", trans_rate, TOTAL_DATA_SIZE/timer);
    trans_rate = TOTAL_DATA_SIZE/timer;

   }
    printf("%lld %lld %lld\n", less, exceed, total);

//   fclose(fp);
	close(sockfd);

       fclose(fp);
    free(buf);
    return 0;
}
