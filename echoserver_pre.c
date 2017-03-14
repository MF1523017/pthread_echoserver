#include"csapp.h"
#include"sbuf.h"
#define NTHREADS 4
#define SBUFSIZE 16

void echo_cnt(int connfd);
void *thread(void *vargp);

sbuf_t sbuf;

int main(int argc,char **argv)
{
        int i,listenfd,connfd;
        char *port;
        socklen_t clientlen=sizeof(struct sockaddr_in);
        struct sockaddr_in clientaddr;
        pthread_t tid;
        if(argc!=2){
                fprintf(stderr,"usage:%s <port>\n",argv[0]);
                exit(0);
        }
        port=argv[1];
        sbuf_init(&sbuf,SBUFSIZE);
        listenfd=Open_listenfd(port);

        for(i=0;i<NTHREADS;++i)
                Pthread_create(&tid,NULL,thread,NULL);

        while(1)
        {
                connfd=Accept(listenfd,(SA*) &clientaddr ,&clientlen);
                sbuf_insert(&sbuf,connfd);
        }
}
void *thread(void *vargp)
{
        Pthread_detach(pthread_self());
        while(1)
        {
                int connfd=sbuf_remove(&sbuf);
                echo_cnt(connfd);
                Close(connfd);
        }
}


static int byte_cnt;
static sem_t mutex;

static void init_echo_cnt(void)
{
        Sem_init(&mutex,0,1);
        byte_cnt=0;
}

void echo_cnt(int connfd)
{
        int n;
        char buf[MAXLINE];
        rio_t rio;
        static pthread_once_t once=PTHREAD_ONCE_INIT;
        
        Pthread_once(&once,init_echo_cnt);
        Rio_readinitb(&rio,connfd);
        
        while((n=Rio_readlineb(&rio,buf,MAXLINE))!=0){
                P(&mutex);
                byte_cnt+=n;
                printf("thread %d received %d (%d total) bytes on fd %d\n",(int)pthread_self(),n,byte_cnt,connfd);
                V(&mutex);
                Rio_writen(connfd,buf,n);
        }
}
