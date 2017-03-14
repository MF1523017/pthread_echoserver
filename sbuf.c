#include"csapp.h"
#include"sbuf.h"
void P(sem_t *s);
void V(sem_t *s);
//create an empty bounded shared FIFO buffer with n slots
//
void sbuf_init(sbuf_t *sp,int n)
{
        sp->buf=Calloc(n,sizeof(int));
        sp->n=n;
        sp->front=sp->rear=0;
        Sem_init(&sp->mutex,0,1);
        Sem_init(&sp->slots,0,n);
        Sem_init(&sp->items,0,0);
}
//clean up buffer sp

void sbuf_deinit(sbuf_t *sp)
{
        Free(sp);
}

//insert item onto the rear of the shared buffer sp
//
void sbuf_insert(sbuf_t *sp,int item)
{
        //begin insert,wait for available slot
        P(&sp->slots);
        P(&sp->mutex);//lock the buffer
        sp->buf[(++sp->rear)%(sp->n)]=item;//insert the item
        V(&sp->mutex);//unlock the buffer
        V(&sp->items);//announce available item
}

//remove and return the first item from buffer sp

int sbuf_remove(sbuf_t *sp)
{
        int item;
        P(&sp->items);//wait for available item
        P(&sp->mutex);//lock the buffer
        item=sp->buf[(++sp->front)%(sp->n)];//remove the item
        V(&sp->mutex);//unlock the buffer
        V(&sp->slots);//announce available slot
        return item;
}

