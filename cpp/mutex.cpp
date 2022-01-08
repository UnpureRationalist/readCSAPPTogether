#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>  

long long gsum = 0;           
long long nelems_per_thread;  
sem_t mutex;  

long long getTimeNs(){ // 获取纳秒级时间 
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}           


void *sum_mutex(void *vargp) 
{
    long long myid = *((long long*)vargp);          
    long long start = myid * nelems_per_thread; 
    long long end = start + nelems_per_thread;  
    long long i;

    for (i = start; i < end; i++) {        
        sem_wait(&mutex);                     
		gsum += i;                    
        sem_post(&mutex);                   
    }	                               
    return NULL;
}


int main(int argc, char **argv) {
    long long i, nelems, log_nelems, nthreads, myid[32];
    pthread_t tid[32];
    
    scanf("%lld", &nthreads);
    scanf("%lld", &log_nelems);
    nelems = (1LL << log_nelems);
    
    nelems_per_thread = nelems / nthreads;
    sem_init(&mutex, 0, 1);

	long long start, end, time;
	start = getTimeNs();
    for (i = 0; i < nthreads; i++) {                  
		myid[i] = i;                                  
		pthread_create(&tid[i], NULL, sum_mutex, &myid[i]); 
    }                                                 
    for (i = 0; i < nthreads; i++)                    
		pthread_join(tid[i], NULL);                   
    end = getTimeNs();
    time = end - start;
    printf("耗时：%lld\n", time);
    
    if (gsum != (nelems * (nelems - 1)) / 2)     
		printf("Error: result = %lld\n", gsum); 
	else
		printf("Correct: result = %lld\n", gsum);

    exit(0);
}




