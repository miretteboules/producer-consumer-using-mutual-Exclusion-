#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define QUEUESIZE 10
#define BUFFERSIZE 10
int n;
int count = 0;
sem_t  counteracces, full, empty, bufferaccess;
typedef struct
{
    int front, rear, num_of_elements;
    int items[QUEUESIZE];
} Queue;
Queue q;

void initialize(Queue *q)
{
    q->front = 0;
    q->rear = -1;
    q->num_of_elements = 0;
}


int isFull(Queue *q)
{
if (q->num_of_elements == QUEUESIZE)
return 1;
else
return 0;
}

int isEmpty(Queue *q)
{
if (q->num_of_elements == 0)
return 1;
else
return 0;
}

void enqueue(Queue *q, int value)
{
    if (!isFull(q))
    {
    q->rear=((q->rear+1)%QUEUESIZE);
    q->items[q->rear]=value;
    q->num_of_elements++;
    //printf("qqqqqqqqqqqqqqqqqqqqqqq++++++++++++++== %d \n",q->num_of_elements);
    }
    else
    printf("Queue is Full \n");
}
int dequeue(Queue *q)
{
 if(isEmpty(q))
    {
    printf("Queue is Empty \n");
    return -1;
    }
else
    {
     q->front=((q->front+1)%QUEUESIZE);
     int value = q->items[q->front];
     q->num_of_elements--;
  //  printf("qqqqqqqqqqqqqqqqqqqqqqq------------------------== %d \n",q->num_of_elements);

     return value;
    }

}

//function gets the count of threads at a time
//count is a shared resource must be putten in a semaphore to guarante mutual exclusion )
void *mCounter(void *x){
int randvar = rand()%8;
sleep(randvar);
printf("Counter Thread %d: recieved a message\n",x);
sem_wait(&counteracces);
printf("Counter Thread %d: waiting to write\n",x);
count++;
printf("Counter Thread %d: now adding to counter, counter value = %d\n",x,count);
sem_post(&counteracces);
}
//function to read the count and access it in the buffer
//(buffer and count are a shared resources )
void *mMonitor(void *a){

while(1){
int randvar = rand()%8;
sleep(randvar);
if(q.num_of_elements==BUFFERSIZE)
{printf("Buffer is full!\n");
}
printf("Monitor Thread: waiting to read counter\n");
sem_wait(&empty);
sem_wait(&bufferaccess);
sem_wait(&counteracces);
enqueue(&q,count);
printf("Monitor Thread: reading a count value of %d\n",count);
printf("Monitor Thread: writing to buffer at position %d\n",q.rear);
count=0;
sem_post(&counteracces);
sem_post(&bufferaccess);
sem_post(&full);
}
pthread_exit(NULL);
}

void *mCollector(void *b){

while(1)
{
int randvar = rand()%7;
 sleep(randvar);
 if(q.num_of_elements==0)
 {printf("Collector Thread: The buffer is empty \n");}
 sem_wait(&full);
 sem_wait(&bufferaccess);
 printf("Collector Thread: Reading from buffer at position %d\n",q.front);
 int temp=dequeue(&q);
 sem_post(&bufferaccess);
 sem_post(&empty);
}
pthread_exit(NULL);
}
int main()
{
sem_init(&empty,0,BUFFERSIZE);
sem_init(&full,0,0);
sem_init(&counteracces,0,1);
sem_init(&bufferaccess,0,1);
initialize(&q);
printf("Enter the number of threads \n");
scanf("%d",&n);


    pthread_t thread1,thread2,thread3[n];
 pthread_create(&thread2, NULL, &mCollector, NULL);
    pthread_create(&thread1, NULL, &mMonitor, NULL);

	for(int i = 0 ; i < n ; i++){
	pthread_create(&thread3[i], NULL, &mCounter, (void *)i);
	}
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	for(int i = 0 ; i < n ; i++){
	pthread_join(thread3[i], NULL);
	}

    return 0;
}
