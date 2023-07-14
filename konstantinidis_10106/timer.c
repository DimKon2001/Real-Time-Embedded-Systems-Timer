#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include "helper.c"


#define PI 3.1415



int n_producers = 1; 
int n_consumers = 4; 
int queueSize = 4; 


//need to update files

//time producer need to insert in queue
FILE * queueInsertTime;
//delay in the producer
FILE * producerDelay;
//time exited queue
FILE * queueExecTime;
FILE * TotalDrift;
FILE * DriftTime;
FILE * delay;
FILE * Compare;
FILE * DriftTime2;
FILE * DriftTime3;
FILE * TotalDrift2;
FILE * TotalDrift3;
FILE * JobsWaitTime;
FILE * FunctionsExecTimes;

//Given struct for defining tne workFuction our consumer threads will execute.
typedef struct {
  void * (*work)(void *);
  void * arg;
  struct timeval timeIn;
} workFunction;

//work function declaration
void * work(void * arg);



//Struct defined for the queue implementation
typedef struct {
  workFunction * buf;
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_mutex_t *prodmut;
  pthread_mutex_t *consmut;
  pthread_cond_t *notFull, *notEmpty;
} queue;


//queue methods (functions) declaration
queue *queueInit (int capacity);
void queueDelete (queue *q);
void queueAdd (queue *q, workFunction in);
void queueExec ( queue *q,workFunction  workFunc,int currHead);
void queueDel (queue *q, workFunction *out);

typedef struct {
  unsigned int Period; 
  unsigned int TasksToExecute; 
  unsigned int StartDelay;
  pthread_t producer_t; 

  void * (* StartFcn)(void *);
  void * (* StopFcn)(void *);
  void * (* ErrorFcn)(void *);
  void * (* TimerFcn)(void *arg);

  void * userData;

  queue *Q;
} Timer;

//timer methods (functions) declarations, based on given specification
Timer * timerCreate (unsigned int Period,unsigned int TasksToExecute, unsigned int StartDelay, void *(*timerFcn)(void *arg));//workFunction * t_TimerFcn);
void timerDelete(Timer * t);
void start(Timer * t);
//void * StartFcn(void * arg);

void startat(Timer * t,int y,int m,int d,int h,int min,int sec);



//Consumer and Producers Threads' functions declaration.
void *producer (void *args);
void *consumer (void *args);

//Function that select the execution mode
int execution_mode();

//variable that helps set the termination condition
int termination;

//OK
//set files used to hold results
void setFiles(){

  //In this file we save the time that the producer needs to insert an assignment to the queue
  char filename1[sizeof "QueueInsertTime.csv"];
  sprintf(filename1, "QueueInsertTime.csv");

  //In these file we save every drift between the actual time of an assignment to the queue and its desired time

  char filename2[sizeof "TotalDrift_1s.csv"];
  sprintf(filename2, "TotalDrift_1s.csv");

  char filename6[sizeof "TotalDrift_01s.csv"];
  sprintf(filename6, "TotalDrift_01s.csv");

  char filename7[sizeof "TotalDrift_001s.csv"];
  sprintf(filename7, "TotalDrift_001s.csv");
  

  //In this file we get the producer's actual Periods between 2 executions
  char filename3[sizeof "DriftTime_1s.csv"];
  sprintf(filename3, "DriftTime_1s.csv");
  char filename4[sizeof "DriftTime_01s.csv"];
  sprintf(filename4, "DriftTime_01s.csv");
  char filename5[sizeof "DriftTime_001s.csv"];
  sprintf(filename5, "DriftTime_001s.csv");


  //In this file we save the time that a job waits in the queue
  char filename8[sizeof "JobWaitTime.csv"];
  sprintf(filename8, "JobWaitTime.csv");

  //In this file we save the time that a job executes
  char filename9[sizeof "FunctionExecTimes.csv"];
  sprintf(filename9, "FunctionExecTimes.csv");

  

  


  //Open the auxilary files, in order to store the desired data

  queueInsertTime = fopen(filename1,"w");
  TotalDrift = fopen(filename2,"w");
  DriftTime = fopen(filename3,"w");
  DriftTime2 = fopen(filename4,"w");
  DriftTime3 = fopen(filename5,"w");
  TotalDrift2 = fopen(filename6,"w");
  TotalDrift3 = fopen(filename7,"w");
  JobsWaitTime = fopen(filename8,"w");
  FunctionsExecTimes = fopen(filename9,"w");

 

  fprintf(queueInsertTime, "data\n");
  fprintf(TotalDrift, "data\n");
  fprintf(DriftTime, "data\n");
  fprintf(DriftTime2, "data\n");
  fprintf(DriftTime3, "data\n");
  fprintf(TotalDrift2, "data\n");
  fprintf(TotalDrift3, "data\n");
  fprintf(JobsWaitTime, "data\n");
  fprintf(FunctionsExecTimes, "data\n");



}

//OK
void closeFiles(){
  fclose(queueInsertTime);
  fclose(DriftTime);
  fclose(DriftTime2);
  fclose(DriftTime3);
  
  fclose(TotalDrift);
  fclose(TotalDrift2);
  fclose(TotalDrift3);

  fclose(JobsWaitTime);
  fclose(FunctionsExecTimes);
}


int main (int argc, char* argv[])
{
  //printf("Max size of int is %lu\n",sizeof(long long int));
  
  int option;
  int n_timers=0;

  
  struct timeval starttime;


  //initializing the queue size and the number of consumers
  initialize_cons_queue(&n_consumers, &queueSize);
  
  //Select one of the 4 program execution modes 
  option = execution_mode();
  
  //initializing the number of producers and the period of the timer
  prog_init(&option, &n_timers,&n_producers);
  
  //open files to store results
  setFiles();

  queue *fifo; //queue declaration
  pthread_t producers[n_producers], consumers[n_consumers]; //pthreads declaration
  Timer * timers[n_timers];


  //initializing termination variable to zero, if it gets equal to the number of producers end producers
  termination = 0;

  //initializing the queue
  fifo = queueInit(queueSize); 
  
  for (int i = 0; i < n_consumers; i++)
    pthread_create (&consumers[i], NULL, consumer, fifo);


  unsigned int periods[3] = {1e3, 0.1*1e3, 0.01*1e3};
  unsigned int TasksToExecute[3] = {3600, 36000, 360000};
  unsigned int StartDelay = 0;


  for(int i=0; i<n_timers; i++){
  

    timers[i]= timerCreate(periods[i+option], TasksToExecute[i+option], StartDelay, &work);//functions[funcIndex]);
 
    
    (timers[i] -> Q)= fifo;
    (timers[i] -> producer_t)=producers[i];

    
    
    //start(timers[i]);
    

    startat(timers[i],2023,7,14,15,38,45);
  }

  
  for(int i=0; i<n_timers; i++){
    pthread_join((timers[i]-> producer_t), NULL);
    (timers[i]-> StopFcn)(NULL);
    timerDelete(timers[i]);
  }

  //We then wait for the consumers to finish their execution
  for (int i = 0; i < n_consumers; i++){
    pthread_join(consumers[i], NULL);
  }

  queueDelete (fifo);

  printf("\nEnd of program.\n");

  return 0;
}


void *producer (void * t)
{
  //Declaration of auxilary time structs for calculating the producer statistics.
  struct timeval execTimestamp;
  struct timeval prevExecTimestamp;
  struct timeval firstTimestamp;
  struct timeval startAssignQueueTime;

  unsigned int actualPeriod;


  int PeriodDrift = 0;
  int TotalPeriodDrift = 0;


  Timer * timer; 
  timer = (Timer *)t;


  unsigned int executions = (timer -> TasksToExecute);
  unsigned int Period = (timer -> Period)*1e3;
  int sleepTime = Period;

  
  unsigned long long sum = 0;

  for(int i = 0; i<executions; i++){
    
    printf("Producer: %d\n", i);


    gettimeofday(&execTimestamp,NULL);

    struct timeval endAssignQueueTime;

    if (i == 0){
      firstTimestamp.tv_sec = execTimestamp.tv_sec;
      firstTimestamp.tv_usec = execTimestamp.tv_usec;
    }
  

    gettimeofday(&startAssignQueueTime,NULL);
    
    
    //critical section begins
    pthread_mutex_lock ((timer-> Q)->mut);
    
    while ((timer-> Q)->full) {
      timer->ErrorFcn(NULL);
      pthread_cond_wait ((timer-> Q)->notFull, (timer-> Q)->mut);

    }
    
    workFunction in;
    in.work = timer->TimerFcn;
    in.arg = (void *)((long)rand()%1000);
    gettimeofday(&(in.timeIn),NULL);


    queueAdd ((timer-> Q), (in));
    
    
    gettimeofday(&endAssignQueueTime,NULL);
    
    

    //end of critical section
    pthread_mutex_unlock ((timer-> Q)->mut);
    pthread_cond_signal ((timer-> Q)->notEmpty);

    
    int timeToAddToQueue = (endAssignQueueTime.tv_sec*1e6 - startAssignQueueTime.tv_sec*1e6 );
    timeToAddToQueue += endAssignQueueTime.tv_usec - startAssignQueueTime.tv_usec;
    

    if(i != 0){
      
      
      sum = sleepTime + sum + TotalPeriodDrift;
      long long int subtract = (execTimestamp.tv_sec*1e6 - firstTimestamp.tv_sec*1e6 );
      subtract += execTimestamp.tv_usec - firstTimestamp.tv_usec;
      
      
      TotalPeriodDrift = subtract - sum ;

      
      sleepTime = timer->Period*1e3 - TotalPeriodDrift;
      sleepTime = sleepTime > 0 ? sleepTime : 0;
      
      
    }


    
    if(i != 0){

      actualPeriod = (execTimestamp.tv_sec*1e6 - prevExecTimestamp.tv_sec*1e6);
      actualPeriod += (execTimestamp.tv_usec - prevExecTimestamp.tv_usec);
      PeriodDrift = actualPeriod - timer->Period*1e3;
    
    }

    prevExecTimestamp = execTimestamp;

    pthread_mutex_lock((timer-> Q)->prodmut);

    fprintf(queueInsertTime,"%d\n", timeToAddToQueue);

    //printf("execTimestamp %f\n", execTimestamp.tv_usec + execTimestamp.tv_sec*1e6);
    //printf("first %f\n", firstTimestamp.tv_usec + firstTimestamp.tv_sec*1e6);
    //if(PeriodDrift>2000)
    //  printf("PeriodDrift %d\n", PeriodDrift);
    //printf("TotalDrift %d\n", TotalPeriodDrift);

    if (timer->Period == 1000){
      fprintf(DriftTime,"%d\n ",(PeriodDrift));
      fprintf(TotalDrift, "%d\n",TotalPeriodDrift);
    }else if (timer->Period == 100){
      fprintf(DriftTime2,"%d\n ",(PeriodDrift));
      fprintf(TotalDrift2, "%d\n",TotalPeriodDrift);
    }else{
      fprintf(DriftTime3,"%d\n ",(PeriodDrift));
      fprintf(TotalDrift3, "%d\n",TotalPeriodDrift);
    }
    pthread_mutex_unlock((timer-> Q)->prodmut);
    
    
    usleep((sleepTime)); 

  }

  pthread_mutex_lock((timer-> Q)-> prodmut);
  termination++;
  pthread_mutex_unlock((timer-> Q)-> prodmut);

  pthread_cond_signal ((timer-> Q)->notEmpty);
  return (NULL);

}


//OK
//termination status
void *consumer (void *q)
{
  queue *fifo;
  fifo = (queue *)q;

  struct timeval jobStartTime;
  struct timeval jobEndTime;

  while (1) {

    workFunction out;

    //critical section begins
    pthread_mutex_lock (fifo->mut);

    
    while (fifo->empty ) {

      
      if(termination == n_producers){

        pthread_mutex_unlock (fifo->mut);
        pthread_cond_signal (fifo->notEmpty);
        return NULL;
      }

      pthread_cond_wait (fifo->notEmpty, fifo->mut);

    }
  
    
    queueDel (fifo, &out);

    //end of critical section
    pthread_mutex_unlock(fifo->mut);
    pthread_cond_signal (fifo->notFull);

    gettimeofday(&jobStartTime,NULL);
    (out.work)((out.arg));

    gettimeofday(&jobEndTime,NULL);
    
    int JobwaitingTime = (jobStartTime.tv_sec*1e6) - (out.timeIn).tv_sec*1e6 + (jobStartTime.tv_usec - (out.timeIn).tv_usec);
    int JobExecutionTime = (jobEndTime.tv_sec*1e6 - jobStartTime.tv_sec*1e6) + (jobEndTime.tv_usec - jobStartTime.tv_usec);

    //mutex for saving data
    pthread_mutex_lock(fifo->consmut);

    fprintf(JobsWaitTime,"%d\n",JobwaitingTime);
    fprintf(FunctionsExecTimes,"%d\n",JobExecutionTime);

    pthread_mutex_unlock(fifo->consmut);
    
  }

  return (NULL);
}
//*/

//OK
queue *queueInit (int size_of_queue)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->buf=(workFunction*) malloc (sizeof(workFunction)*size_of_queue);
  if((q->buf) == 0){
    return NULL;
  }

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  //extra mutexes for saving data
  q->prodmut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->prodmut, NULL);
  q->consmut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->consmut, NULL);

  return (q);
}

//OK
void queueDelete (queue *q)
{
  free (q->buf);
  pthread_mutex_destroy (q->mut);
  free (q->mut);
  pthread_mutex_destroy(q->prodmut);
  free (q->prodmut);
  pthread_mutex_destroy(q->consmut);
  free (q->consmut);  
  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  
  free (q);
}

//OK
void queueAdd (queue *q, workFunction in)
{

  q->buf[q->tail] = in;

  q->tail++;
  if (q->tail == queueSize)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

//OK
void queueDel (queue *q, workFunction *out)
{

  *out = q->buf[q->head];
  
  q->head++;
  if (q->head == queueSize)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;

}


/*
void * StartFcn(void * arg, unsigned int Period, unsigned int TasksToExecute, unsigned int StartDelay, void *(*timerFcn)(void *arg)){
  timerCreate(Period, TasksToExecute, StartDelay, timerFcn);
  printf("Timer set.\n");
  return NULL;
}
*/

void * StartFcn(void * arg){
  printf("Timer set.\n");
  return NULL;
}

/*void * StopFcn(Timer * timer){
  free(timer);
}
*/

void * StopFcn(void * arg){
  printf("Timer has stopped executed\n");
  return NULL;
}

//OK
void * ErrorFcn(void * arg){
  print_debug(0);
  return NULL;
}
//OK

Timer * timerCreate(unsigned int Period, unsigned int TasksToExecute, unsigned int StartDelay, void *(*timerFcn)(void *arg)){

  Timer *timer;
  timer = (Timer *)malloc (sizeof (Timer));

  if (!timer){
    return (NULL);
  }

  timer->Period = Period;
  timer->TasksToExecute = TasksToExecute;
  timer->StartDelay = StartDelay;

  timer->TimerFcn = timerFcn;


  timer->StartFcn = &StartFcn ;

  timer->StopFcn = &StopFcn;
  timer->ErrorFcn = &ErrorFcn;

  timer->userData = NULL;

  return (timer);
}

//OK
void timerDelete(Timer * timer){

  free(timer);
}

//OK  
void start(Timer * timer){

  (timer-> StartFcn)(NULL);
  pthread_t producer_t;

  if(timer->StartDelay){

    printf("Waiting for %d seconds.\n", timer->StartDelay);
    usleep((timer->StartDelay * 1e6));
    
  } 

  pthread_create(&(timer->producer_t) , NULL, producer, timer);


}


void * work(void * arg){
  
  float sin_arg;
  float cos_arg;

  int x = (int) arg;
  //printf("The arg is %d\n", x);
  //printf("3  ");
  //print_debug(1);
  
  sin_arg = sin((float)((x * PI) / 180));
  cos_arg = cos((float)((x * PI) / 180));
  
  
  return NULL;
}


void startat(Timer * timer ,int y,int m,int d,int h,int min,int sec){

  time_t now = time(NULL);

    struct tm* current_time = localtime(&now);

    current_time->tm_year = y - 1900;
    current_time->tm_mon = m - 1;
    current_time->tm_mday = d;
    current_time->tm_hour = h;
    current_time->tm_min = min;
    current_time->tm_sec = sec;

    time_t target = mktime(current_time);

    printf("Executing at local time and date: %s", asctime (current_time) );
    printf("Target time: %s", asctime (localtime(&now)));

    if (target == -1) {
        printf("Invalid target time.\n");
        return;
    }

    double seconds_diff = difftime(target, now);

    if (seconds_diff <= 0) {
        printf("Target time has already passed.\n");
        seconds_diff = 0;
    }

    printf("Waiting for %lf seconds...\n", seconds_diff);

    sleep((unsigned int)seconds_diff);


    pthread_create (&(timer->producer_t) , NULL, producer, timer);

}