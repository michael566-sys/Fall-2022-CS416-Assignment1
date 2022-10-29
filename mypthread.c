// File:	mypthread.c

// List all group members' names:
// iLab machine tested on:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE

#define QUANTUM 10                          // in ms



// YOUR CODE HERE
//_Atomic 


static inline int left_child(int index)
{
    return (2 * index + 1);
}

static inline int right_child(int index)
{
    return (2 * index + 2);
}

static inline int parent(int index)
{
    return ((index - 1) / 2);
}


min_heap heap_init(int capacity)
{
    min_heap heap = (min_heap)
    {
        .capacity = capacity, 
        .size = 0
    };
    return heap;
}

static inline void swap(tcb *t1, tcb *t2)
{
    tcb temp = *t1;
    *t1 = *t2;
    *t2 = temp;
}


void heapify(min_heap *heap, int index)
{
    int min = 0;
    if(left_child(index) < heap->size && heap->data[left_child(index)].priority < heap->data[index].priority)
    {
        min = left_child(index);
    }
    else
    {
        min = index;
    }

    if(right_child(index) < heap->size && heap->data[right_child(index)].priority < heap->data[min].priority)
    {
        min = right_child(index);
    }

    if(min != index)
    {
        swap(&(heap->data[index]), &(heap->data[min]));
        heapify(heap, min);
    }
}


void insert(min_heap *heap, tcb data)
{
    if(heap->size && heap->capacity)
    {
        if(heap->size >= heap->capacity)
        {
            heap->capacity *= 2;
            heap->data = realloc( heap->data, sizeof (tcb) * heap->capacity );
        }
    }
    else if(!heap->capacity)
    {
        heap->capacity ++;
        heap->data = malloc( sizeof (tcb) * heap->capacity );
    }

    tcb temp = { 0 };
	memcpy( &temp, data,  sizeof (tcb) );

    
    int index = heap->size;
    heap->size++;
    while(index && temp.priority < heap->data[parent(index)].priority)
    {
        heap->data[index] = heap->data[parent(index)];
        index = parent(index);
    }
    heap->data[index] = temp;
    //heapify(heap, 0);
}


tcb pop_first(min_heap *heap)
{
    tcb temp = { 0 };
    if(heap->size)
    {
        temp = { 0 };
		memcpy( &temp, &heap->data[0], sizeof (tcb) );
		
        heap->data[0] = heap->data[-- heap->size];
        heapify(heap, 0);

        if(heap->size < heap->capacity / 4)
        {
            heap->capacity /= 2;
            heap->data = realloc(heap->data, sizeof(tcb) * heap->capacity);
        }
        return temp;
    }
    //fprintf(stderr, "The heap is already empty\n");
    heap->capacity = 0;
    free(heap->data);
    heap->data = NULL;  //in case a double free happens
    return temp;        
}









//global variable
static _Atomic struct itimerval timer = { 0 };
static bool init = false;
static min_heap thread_pool = { 0 };
uint8_t tid_counter = 0;
ucontext_t scheduler_context = { 0 };
ucontext_t main_context = { 0 };




/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg)
{
	// YOUR CODE HERE	

	// create a Thread Control Block
	// create and initialize the context of this thread
	// allocate heap space for this thread's stack
	// after everything is all set, push this thread into the ready queue

	return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield()
{
	// YOUR CODE HERE
	
	// change current thread's state from Running to Ready
	// save context of this thread to its thread control block
	// switch from this thread's context to the scheduler's context

	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr)
{
	// YOUR CODE HERE

	// preserve the return value pointer if not NULL
	// deallocate any dynamic memory allocated when starting this thread
	
	return;
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr)
{
	// YOUR CODE HERE

	// wait for a specific thread to terminate
	// deallocate any dynamic memory created by the joining thread

	return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
	// YOUR CODE HERE
	
	//initialize data structures for this mutex
	



	return 0;
};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE

	// use the built-in test-and-set atomic function to test the mutex
	// if the mutex is acquired successfully, return
	// if acquiring mutex fails, put the current thread on the blocked/waiting list and context switch to the scheduler thread
	
	return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE	
	
	// update the mutex's metadata to indicate it is unlocked
	// put the thread at the front of this mutex's blocked/waiting queue in to the run queue

	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE
	
	// deallocate dynamic memory allocated during mypthread_mutex_init

	return 0;
};

/* scheduler */
static void schedule( int sig )
{
	// YOUR CODE HERE
	
	// each time a timer signal occurs your library should switch in to this context
	
	// be sure to check the SCHED definition to determine which scheduling algorithm you should run
	//   i.e. RR, PSJF or MLFQ


	return;
}

/* Round Robin scheduling algorithm */
static void sched_RR()
{
	// YOUR CODE HERE
	
	// Your own implementation of RR
	// (feel free to modify arguments and return types)
	
	return;
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF()
{
	// YOUR CODE HERE

	// Your own implementation of PSJF (STCF)
	// (feel free to modify arguments and return types)

	return;
}

/* Preemptive MLFQ scheduling algorithm */
/* Graduate Students Only */
static void sched_MLFQ() {
	// YOUR CODE HERE
	
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	return;
}

// Feel free to add any other functions you need

// YOUR CODE HERE
