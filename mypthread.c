// File:	mypthread.c

// List all group members' names:
// iLab machine tested on:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE

#define QUANTUM 20000                         // in 20 ms
#define LOCK 1  //used to represent LOCKED
#define UNLOCK 0    //used to represent UNLOCKED
#define STACK_SIZE 1048576 //1 MB stack size

tcb* current_thread = NULL;

queue *reeady_queue = NULL;
//global variable

struct itimerval timer;
static bool init = false;
static min_heap thread_pool = { 0 };
uint8_t tid_counter = 0;
ucontext_t scheduler_context = { 0 };
ucontext_t main_context = { 0 };

//initialize the timer
void init_timer() {
    // timer start immediately
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;

    // timer interval
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = QUANTUM;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

// restart the timer
void start_timer(){
    setitimer(ITIMER_VIRTUAL, &timer, 0);
}

// stop the timer
void stop_timer() {
    setitimer(ITIMER_VIRTUAL, 0, 0);
}


/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg)
{
	// YOUR CODE HERE	
    
	// create a Thread Control Block
	// create and initialize the context of this thread
	// allocate heap space for this thread's stack
	// after everything is all set, push this thread into the ready queue
    
    // initialize the ready_queue if it's empty 
    if(reeady_queue == NULL){
        reeady_queue = queue_init();
    }
    tcb *thread = (tcb*)malloc(sizeof(tcb));




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
    if(mutex == NULL) return 1;
	mutex->wait_queue = queue_init();
    mutex->owner = NULL;
    mutex->status = UNLOCK;
	return 0;
};

/* aquire a mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE

	// use the built-in test-and-set atomic function to test the mutex
	// if the mutex is acquired successfully, return
	// if acquiring mutex fails, put the current thread on the blocked/waiting list and context switch to the scheduler thread
    if(mutex == NULL) return 1;

    while(__sync_lock_test_and_set(&(mutex->status), LOCK) == LOCK) {
        current_thread->status = waiting;
        // put the thread on the blocking/waiting
        enqueue(mutex->wait_queue, current_thread);
        // switch to scheduler thread
        mypthread_yield();
    }

    current_thread->status = running;
    mutex->owner = current_thread;
	return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE	
	
	// update the mutex's metadata to indicate it is unlocked
	// put the thread at the front of this mutex's blocked/waiting queue in to the run queue
    if(mutex == NULL) return 1;

    tcb *tcb = dequeue(mutex->wait_queue);
    // no waiting thread
    if(tcb == NULL) {
        mutex->status = UNLOCK;
    }else{
        // set the ower of the mutex to the next thread
        mutex->owner = tcb;
        // change the status of the next thread to waiting
        tcb->status = running;
        
    } 
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex)
{
	// YOUR CODE HERE
	// deallocate dynamic memory allocated during mypthread_mutex_init
    if(mutex == NULL) return 1;
    if(mutex->status == LOCK) return 1; // the mutex is in use
    free(mutex->wait_queue); // free the allocated memory
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


//queue initialization
queue* queue_init() {
    queue* q = (queue*) malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

// Append the new node to the back of the queue
void enqueue (queue* queue, tcb *tcb ) {
    queue_node* queue_node = malloc(sizeof(queue_node));
    queue_node -> tcb = tcb;
    queue_node -> next = NULL; // At back of the queue, there is no next node.

    if (queue->tail == NULL) { // If the Queue is currently empty
        queue->head = queue_node;
        queue->tail = queue_node;
    } else {
        queue->tail->next = queue_node;
        queue->head = queue_node;
    }
    ++(queue->size);
    return;
}

// Remove a QueueNode from the front of the Queue
tcb* dequeue (queue* queue ) {

    if (queue->size == 0) { // If the Queue is currently empty
        return NULL;
    } else {

        // The QueueNode at front of the queue to be removed
        queue_node* temp = queue->head;
        tcb* tcb = temp->tcb;

        queue->head = temp->next;
        --(queue->size);
        if (queue->tail == temp) { // If the Queue will become empty
            queue->tail = NULL;
        }

        free(temp);
        return tcb;
    }
}
