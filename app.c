#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include "memalloc.c"

void *memoryThreads(){
	
	//pthread_mutex_lock(&lock);
	int r = rand() % 1000;
	void *x1, *x2, *x3,*x4,*x5,*x6;
	printf("RRR: %d\n", r);
	x1 = mem_allocate(r);
	//mem_print();

  	//pthread_mutex_unlock(&lock);
  	pthread_exit(0);
}


int main(int argc, char *argv[]){
	void *chunkptr;
	void *endptr;
	char *charptr;
	int ret;
	int i;
	int size;
	void *x1, *x2, *x3, *x4, *x5,*x6,*x7;	// object pointers
	
	if (argc != 2) {
		printf("usage: app <size in KB>\n");
		exit(1);
	}
	
	size = atoi(argv[1]); // unit is in KB
	
	// allocate a chunk
	chunkptr = sbrk(0); // end of data segment
	sbrk(size * 1024);	// extend data segment by indicated amount (bytes)
	endptr = sbrk(0);	// new end of data segment
	
	printf("chunkstart=%lx, chunkend=%lx, chunksize=%lu bytes\n", (unsigned long)chunkptr, (unsigned long)endptr, (unsigned long)(endptr - chunkptr));
	
	//test the chunk
	printf("---starting testing chunk\n");
	charptr = (char *)chunkptr;
	
	for (i = 0; i < size; ++i)
		charptr[i] = 0;
	
	printf("---chunk test ended - success\n");
	
	ret = mem_init(chunkptr, size, 0);
	if (ret == -1) {
		printf("could not initialize \n");
		exit(1);
	}
	
	mem_print();
	
	//to measure the time
	struct timeval t1, t2;	
	long elapsed_time;
	gettimeofday(&t1, NULL);
	
	//----- TEST 0 -----
	/*int n = 100;
	
	pthread_t tid[n];
    pthread_attr_t attr[n];
    
    for(i = 0; i < n; i++){
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], &attr[i], memoryThreads, NULL);
    }
    
    for(i = 0; i < n; i++){
        pthread_join(tid[i], NULL);
    }*/
	
	//----- TEST 1 -----//
	// below we allocate and deallocate memory dynamically
	/*int n = 1000;
	void *pointer[n];
	
	for(int i = 0; i < n; i++){
		int r = rand() % n;
		//printf("%d\n", r);
		pointer[i] = mem_allocate(r);
	}
	//mem_print();
	
	for(int i = 1; i < n; i = i+2){
		mem_free(pointer[i]);
	}
	//mem_print();
	pthread_t tid[n];
    pthread_attr_t attr[n];
	for(i = 0; i < n/2; i++){
        pthread_attr_init(&attr[i]);
        pthread_create(&tid[i], NULL, memoryThreads, NULL);
    }
    
    for(i = 0; i < n/2; i++){
        pthread_join(tid[i], NULL);
    }
	
	for(int i = 1; i < n; i = i+2){
		int r = rand() % 1000;
		pointer[i] = mem_allocate(r); 
	}*/
	
	//----- TEST 2-----// 
	printf("allocation of 300 :\n");
	x1 = mem_allocate(300);
	x2 = mem_allocate(1000);
	x3 = mem_allocate(600);
	x4 = mem_allocate(700);
	x5 = mem_allocate(380);
	mem_print();
	
	printf("deletion of 700 :\n");
	mem_free(x4);
	mem_print();
	
	printf("deletion of 1000 :\n");
	mem_free(x2);
	mem_print();
	
	printf("allocation of 7000 :\n");
	x6 = mem_allocate(7000);
	mem_print();
	
	printf("allocation of 500 :\n");
	x7 = mem_allocate(500);
	mem_print();
	
	printf("deletion of 380 :\n");
	mem_free(x5);
	mem_print();
	
	//to measure the time 
	gettimeofday(&t2, NULL);
	printf("Duration MS %'.3f\n", (double) (t2.tv_sec - t1.tv_sec) * 1000 + (double) (t2.tv_usec - t1.tv_usec) / 1000);

	return 0;
}
