#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAXINT 2000000000
#define MININT -2000000000

/*Global Variables */
void* startPtr; // points to the start of memory
int   memSize;  // size of memory

pthread_mutex_t lock;
void *empty, *full; //[start address, next address, size]

int algorithm;
int extra = 2 * sizeof(unsigned long) + sizeof(int);

// getters
void *getStart(void *ptr){
	return ptr;
}

void *getNext(void *ptr){
	return (void *) *((unsigned long *) (ptr + sizeof(unsigned long)));
}

int getSize(void *ptr){
	return *((int *) (ptr + 2 * sizeof(unsigned long)));
}

// setters
void setStart(void *ptr, void *start){
	*((unsigned long *) ptr) = (unsigned long) start;
}

void setNext(void *ptr, void *next){
	*((unsigned long *) (ptr + sizeof(unsigned long))) = (unsigned long) next;
}

void setSize(void *ptr, int size){
	*((int *) (ptr + 2 * sizeof(unsigned long))) = size;
}
// printfs are for debugging; remove them when you use/submit your library
int mem_init (void *chunkpointer, int chunksize, int method){
	
	chunksize = 1024 * chunksize;
	
	//printf("init called1\n");
	if(chunkpointer != NULL && chunksize >= 0 && method>-1 && method < 3){
		startPtr = chunkpointer; // points to the start of memory 
		memSize = chunksize;  // size of memory
		full = NULL;
		algorithm = method;
		
	//	printf("Empty getStart:%p\n",empty);
		empty=startPtr;		
	//	printf("GetStart(empty):%p , empty %p\n",getStart(empty), empty);
		setNext(empty,NULL);		
		setSize(empty,memSize-extra);	
	//	printf("getSize(empty) %d, memSize %d \n", getSize(empty),memSize);
	//	printf("getNext(empty) %p \n", getNext(empty));
		//pthread_mutex_init(&lock, NULL);	
		return (0);
	}
	else{
		printf("Parameters are wrong!\n");
		return -1;
	}
}
void *mem_allocate (int objectsize){
	pthread_mutex_lock(&lock);
	void *res = NULL;
	
	if(empty){	
		//first fit
		if(algorithm == 0){
			//printf("---FIRST FIT---");
			//found big enough empty chunk
			void *cur = empty, *prev = NULL;
			int found = 0; 
			while(cur != NULL && !found){
				if(getSize(cur) >= objectsize + extra){
					found = 1;
				}
				else{ 
					prev = cur;
					cur = getNext(cur);
				}
			}
			if(found){
				void *next = getNext(cur);
				if(getSize(cur) >= objectsize + 2 * extra){
					// update empty list
					void *hole = cur + objectsize + extra;
					if(next) setNext(hole, next);
					else setNext(hole,NULL);
					if(prev) setNext(prev,hole);
					else empty = hole;
					setSize(hole, getSize(cur)-(objectsize+extra));
					
					//update full list
					void* cur2=full,*prev2 = NULL;
					while(cur2 != NULL && getStart(cur) > getStart(cur2)){	
						prev2 = cur2;
						cur2 = getNext(cur2);
					}
					if(cur2) setNext(cur,cur2);
					else setNext(cur,NULL);
					if(prev2) setNext(prev2, cur);
					else full = cur;
					setSize(cur,objectsize);
					
					res = cur + extra;
				}
				//cuk oturma case i
				else{
					// update empty list
					if(prev) setNext(prev,next);
					else if(next) empty = next;
					else empty = NULL;
					
					//update full list
					void* cur2=full,*prev2 = NULL;
					while(cur2 != NULL && getStart(cur) > getStart(cur2)){	
						prev2 = cur2;
						cur2 = getNext(cur2);
					}
					if(cur2) setNext(cur,cur2);
					else setNext(cur,NULL);
					if(prev2) setNext(prev2, cur);
					else full = cur;
					setSize(cur,objectsize);
					
					res = cur + extra;
				}
			}
			else{
				printf("There's no available space left\n");
			}
		}
		//best fit
		if(algorithm == 1){
			//printf("---BEST FIT---");
			//find the best hole in empty list
			void *cur = empty, *prev = NULL;
			void *temp_cur = NULL;	//temp for cur
			void *temp_prev = NULL;	//temp for prev 
			int found = 0; 
			int diff = MAXINT;
			while(cur != NULL){
				if(getSize(cur) >= objectsize + extra){
					int temp = getSize(cur) - (objectsize + extra);
					if(temp < diff){
						diff = temp;
						//printf("DIFF: %d\n", diff);
						temp_cur = cur;
						temp_prev = prev;
						found = 1;
					}
				}		
				prev = cur;
				cur = getNext(cur);
			}
			
			if(found){
				void *next = getNext(temp_cur);
				if(getSize(temp_cur) >= objectsize + 2 * extra){
					// update empty list
					void *hole = temp_cur + objectsize + extra;
					if(next) setNext(hole, next);
					else setNext(hole,NULL);
					if(temp_prev) setNext(temp_prev,hole);
					else empty = hole;
					setSize(hole, getSize(temp_cur)-(objectsize+extra));
					
					//update full list
					void* cur2=full,*prev2 = NULL;
					while(cur2 != NULL && getStart(temp_cur) > getStart(cur2)){	
						prev2 = cur2;
						cur2 = getNext(cur2);
					}
					if(cur2) setNext(temp_cur,cur2);
					else setNext(temp_cur,NULL);
					if(prev2) setNext(prev2, temp_cur);
					else full = temp_cur;
					setSize(temp_cur,objectsize);
					
					res = temp_cur + extra;
				}
				//cuk oturma case i
				else{
					// update empty list
					if(temp_prev) setNext(temp_prev,next);
					else if(next) empty = next;
					else empty = NULL;
					
					//update full list
					void* cur2=full,*prev2 = NULL;
					while(cur2 != NULL && getStart(temp_cur) > getStart(cur2)){	
						prev2 = cur2;
						cur2 = getNext(cur2);
					}
					if(cur2) setNext(temp_cur,cur2);
					else setNext(cur,NULL);
					if(prev2) setNext(prev2, temp_cur);
					else full = temp_cur;
					setSize(temp_cur,objectsize);
					
					res = temp_cur + extra;
				}
			}
			else{
				printf("There's no available space left\n");
			}
			
		}
		
		//worst fit
		if(algorithm == 2){
			//printf("---WORST FIT---");
			//find the worst hole in empty list
			void *cur = empty, *prev = NULL;
			void *temp_cur = NULL;	//temp for cur
			void *temp_prev = NULL;	//temp for prev 
			int found = 0; 
			int diff = MININT;
			while(cur != NULL){
				if(getSize(cur) >= objectsize + extra){
					int temp = getSize(cur) - (objectsize + extra);
					if(temp > diff){
						diff = temp;
						//printf("DIFF: %d\n", diff);
						temp_cur = cur;
						temp_prev = prev;
						found = 1;
					}
				}		
				prev = cur;
				cur = getNext(cur);
			}
			
			if(found){
				void *next = getNext(temp_cur);
				if(getSize(temp_cur) >= objectsize + 2 * extra){
					// update empty list
					void *hole = temp_cur + objectsize + extra;
					if(next) setNext(hole, next);
					else setNext(hole,NULL);
					if(temp_prev) setNext(temp_prev,hole);
					else empty = hole;
					setSize(hole, getSize(temp_cur)-(objectsize+extra));
					
					//update full list
					void* cur2=full,*prev2 = NULL;
					while(cur2 != NULL && getStart(temp_cur) > getStart(cur2)){	
						prev2 = cur2;
						cur2 = getNext(cur2);
					}
					if(cur2) setNext(temp_cur,cur2);
					else setNext(temp_cur,NULL);
					if(prev2) setNext(prev2, temp_cur);
					else full = temp_cur;
					setSize(temp_cur,objectsize);
					
					res = temp_cur + extra;
				}
				//cuk oturma case i
				else{
					// update empty list
					if(temp_prev) setNext(temp_prev,next);
					else if(next) empty = next;
					else empty = NULL;
					
					//update full list
					void* cur2=full,*prev2 = NULL;
					while(cur2 != NULL && getStart(temp_cur) > getStart(cur2)){	
						prev2 = cur2;
						cur2 = getNext(cur2);
					}
					if(cur2) setNext(temp_cur,cur2);
					else setNext(cur,NULL);
					if(prev2) setNext(prev2, temp_cur);
					else full = temp_cur;
					setSize(temp_cur,objectsize);
					
					res = temp_cur + extra;
				}
			}
			else{
				printf("There's no available space left\n");
			}
		}
		//buraya kadar
	}
	else{
		printf("There's no available space left\n");
	}
	
	pthread_mutex_unlock(&lock);
	
	return res;
}
void mem_free(void *objectptr){
	pthread_mutex_lock(&lock);
	
	void* cur = objectptr-extra;
	//ful list te curr prev ara
	void* temp = full,*prev = NULL;
	while(temp != NULL && getStart(cur) != getStart(temp)){	
		prev = temp;
		temp = getNext(temp);
	}
	//if temp==NULL bulunamadı reyis
	if(temp == NULL){
		printf("Cannot find pointer\n");
		pthread_mutex_unlock(&lock);
		return;
	}
	//if prev == null full deki ilk node
	else if(prev == NULL)
	{
		//ilk ve tek değilse full un başına geçir
		if(getNext(temp))
		{
			 full = getNext(temp);
			 int s= getSize(temp);
			 printf("SSSS: %d, %d\n", s, getSize(empty));
			 if(empty) setNext(temp, empty);
			 //setSize(empty,getSize(empty));
			 empty = temp;		 
		}
		//ilk ve sonsa tüm memory i empty yap
		else
		{
			empty=startPtr;		
			setNext(empty,NULL);		
			setSize(empty,memSize-extra);
			full = NULL;	
		}
		//update empty list
		
		pthread_mutex_unlock(&lock);
		return;
	}
	//Empty listte prev ve temp nextlerini ara
	void * temp2 = empty, *prevtemp2=NULL;
	void *prevNext = getStart(prev)+ getSize(prev)+ extra;
	void *tempNext = getStart(temp)+ getSize(temp) + extra; 
	int left=0, right=0,found=0;
	//printf("temp2: %p, prevNext: %p, tempNext: %p, prev %p\n", temp2,prevNext, tempNext,prev);	
	while(temp2 != NULL&& found == 0)
	{
		if(prevNext == getStart(temp2)) left++;
		if(tempNext == getStart(temp2)){ right++; break;}
		prevtemp2=temp2;
		temp2 = getNext(temp2);
	}
	//printf("left: %d, right: %d\n", left, right);
	//if prev.net != NULL temp.next == NULL
	//	soldan merge
	if(left == 1 && right == 0)
	{
		//printf("soldan\n");
		setSize(prevNext, getSize(prevNext)+getSize(temp)+extra);
		if(getNext(temp)) setNext(prev,getNext(temp));
		else setNext(prev, NULL);
	}
	//if  prev.net == NULL temp.next != NULL
	//	sağdan merge
	if(left == 0 && right == 1)
	{
		//printf("sağdan\n");
		if(prev) setNext(prev, getNext(temp));
		else setNext(prev,NULL);
		//empty
		//printf("temp: %p, tempnext:%p \n",temp,getNext(temp));
		if(getNext(temp2)) setNext(temp,getNext(temp2));
		else setNext(temp,NULL);
		//printf("temp: %p, tempnext:%p \n",temp,tempNext);
		//printf("prevtemp2= %p,temp %p \n",prevtemp2,temp);
		if(prevtemp2)
		{
			prevtemp2=temp;
			setSize(temp, getSize(tempNext)+getSize(temp)+extra);
		}
		else
		{
			empty=temp;
			setSize(temp, getSize(tempNext)+getSize(temp)+extra);
		}
		
		pthread_mutex_unlock(&lock);
		return;
	}
	//prev.net != NULL temp.next != NULL
	//	soldan sağdan merge
	if(left == 1 && right == 1)
	{
		//printf("-soldan sağdan-\n");
		setSize(prevNext, getSize(prevNext)+getSize(tempNext)+2*extra);
		setNext(prevNext, getNext(tempNext));
		if(prev) setNext(prev, getNext(temp));
		else setNext(prev,NULL);
		
		pthread_mutex_unlock(&lock);
		return;
	}
	//prev.net == NULL temp.next == NULL
	// merge yok
	if(left == 0 && right == 0)
	{
		//printf("cuk oturan\n");
		setNext(prev, getNext(temp));

		void* c=empty,*prevc = NULL;
		while(c != NULL && getStart(temp) > getStart(c)){	
			prevc = c;
			c = getNext(c);
		}
		//printf("prevc: %p, c %p \n", prevc, c);
		//full list update
		if(getNext(temp)) setNext(prev,getNext(temp));
		else setNext(prev,NULL);
		//printf("getNext(prevc): %p\n", getNext(temp));
		
		if(prevc)
		{
			setNext(temp,c);
			setNext(prevc,temp);		
		} 
		if(!prevc)
		{
			//printf("temp: %p, c: %p \n", temp,c);
			setNext(temp,c);
			//printf("temp: %p, c: %p \n", temp,c);
			empty=temp;
			//printf("temp: %p, empty: %p \n", temp,empty);
		} 
		//printf("empty: %p", empty);
		pthread_mutex_unlock(&lock);
		return;
	}
	pthread_mutex_unlock(&lock);
	return;
	//pthread_mutex_unlock(&lock);
}
void mem_print (){
	pthread_mutex_lock(&lock);

	void *cur = empty;
	void *cur2 = full;

	while(cur != NULL){
		printf("Empty Start: %lx, E End: %lx, E Size: %d\n", (unsigned long) (cur), (unsigned long) (cur + getSize(cur)), (getSize(cur)));
		cur = getNext(cur);
	}
	
	while(cur2 != NULL){
		printf("Full Start: %lx, F End: %lx, F Size: %d\n", (unsigned long) (cur2), (unsigned long) (cur2 + getSize(cur2)), (getSize(cur2)));
		cur2 = getNext(cur2);
	}
	printf("\n");
	
	pthread_mutex_unlock(&lock);
}

