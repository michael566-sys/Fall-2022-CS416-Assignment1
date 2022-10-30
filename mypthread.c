// File:	mypthread.c

// List all group members' names:
// iLab machine tested on:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE

#define QUANTUM 10                          // in ms
#define LOCK 1  //used to represent LOCKED
#define UNLOCK 0    //used to represent UNLOCKED
#define STACK_SIZE 1048576 //1 MB stack size


//global variable
static struct sigaction sa = { 0 };
static struct itimerval timer = { 0 };
static bool init = false;

#ifdef PSJF
static min_heap *thread_pool = NULL;
#endif 	//PSJF

#ifdef RR
static queue *thread_pool = NULL；
#endif 	//RR

mypthread_t tid_counter = 0;
ucontext_t scheduler_context = { 0 };
ucontext_t main_context = { 0 };
tcb *main_tcb = NULL;
tcb *curr_tcb = NULL;




// YOUR CODE HERE
//_Atomic 

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
		queue_node->prev = queue->tail;
        queue->tail->next = queue_node;
        queue->tail = queue_node;
    }
    (queue->size)++;
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
    if(left_child(index) < heap->size && heap->tcb[left_child(index)].priority < heap->tcb[index].priority)
    {
        min = left_child(index);
    }
    else
    {
        min = index;
    }

    if(right_child(index) < heap->size && heap->tcb[right_child(index)].priority < heap->tcb[min].priority)
    {
        min = right_child(index);
    }

    if(min != index)
    {
        swap(&(heap->tcb[index]), &(heap->tcb[min]));
        heapify(heap, min);
    }
}


void insert(min_heap *heap, tcb *thread)
{
    if(heap->size && heap->capacity)
    {
        if(heap->size >= heap->capacity)
        {
            heap->capacity *= 2;
            heap->tcb = realloc( heap->tcb, sizeof (tcb) * heap->capacity );
        }
    }
    else if(!heap->capacity)
    {
        heap->capacity++;
        heap->tcb = malloc( sizeof (tcb) * heap->capacity );
    }

    tcb temp = { 0 };
	memcpy( &temp, thread, sizeof (tcb) );

    
    int index = heap->size;
    heap->size++;
    while(index && temp.priority < heap->tcb[parent(index)].priority)
    {
        heap->tcb[index] = heap->tcb[parent(index)];
        index = parent(index);
    }
    heap->tcb[index] = temp;
    //heapify(heap, 0);
}


tcb pop_first(min_heap *heap)
{
    tcb temp = { 0 };
    if(heap->size)
    {
		memcpy( &temp, &heap->tcb[0], sizeof (tcb) );
		
        heap->tcb[0] = heap->tcb[-- heap->size];
        heapify(heap, 0);

        if(heap->size < heap->capacity / 4)
        {
            heap->capacity /= 2;
            heap->tcb = realloc(heap->tcb, sizeof(tcb) * heap->capacity);
        }
        return temp;
    }
    //fprintf(stderr, "The heap is already empty\n");
    heap->capacity = 0;
    free(heap->tcb);
    heap->tcb = NULL;  //in case a double free happens
    return temp;        
}








/* create a new thread */
static void wrapper( tcb* thread )
{
	void* result = thread->func( thread->args );
	thread->return_val = result;
	thread->status = finished;
	return;
}
static void schedule( int sig );
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg)
{
	// YOUR CODE HERE	

	// create a Thread Control Block
	// create and initialize the context of this thread
	// allocate heap space for this thread's stack
	// after everything is all set, push this thread into the ready queue

	if ( !init )
	{
		//get main context
		if ( getcontext ( &main_context ) == -1 )
		{
			return -1;
		}
		main_context.uc_link = &scheduler_context;
		//main_context.uc_stack.ss_sp = malloc( 10 * STACK_SIZE );
		//main_context.uc_stack.ss_size = 10 * STACK_SIZE;
		main_tcb = malloc( sizeof (tcb) );
		*main_tcb = (tcb)
		{
			.tid = tid_counter++,
			.status = ready,
			.context = &main_context,
			.priority = 0
		};
		curr_tcb = main_tcb;

		//thread_pool init
		#ifdef PSJF
		*thread_pool = heap_init( 5 );

		#endif 	// PSJF
		#ifdef RR
		thread_pool = queue_init();
		//enqueue( thread_pool, main_tcb )
		#endif 	// RR
		if ( getcontext( &scheduler_context ) == -1 )
		{
			return -1;
		}
		scheduler_context.uc_link = 0;
		scheduler_context.uc_stack.ss_sp = malloc( STACK_SIZE );
		scheduler_context.uc_stack.ss_size = STACK_SIZE;

		makecontext( &scheduler_context, schedule, 1, NULL );

		//set signal handler and timer
		sa.sa_handler = &schedule;
		sigaction( SIGVTALRM, &sa, NULL );

		timer = ( struct itimerval )
		{
			.it_value.tv_usec 	= QUANTUM,
		};
		setitimer( ITIMER_VIRTUAL, &timer, NULL );

		init = true;
	}

	tcb* temp = malloc( sizeof (tcb) );
	*temp = (tcb) 
	{ 
		.tid = tid_counter++,
		.status = waiting,
		.context = malloc( sizeof (ucontext_t) ),
		.func = function,
		.args = arg
	};
	if ( getcontext( temp->context ) == -1 )
	{
		return -1;
	}
	temp->context->uc_link = &scheduler_context;
	temp->context->uc_stack.ss_sp = malloc( STACK_SIZE );
	temp->context->uc_stack.ss_size = STACK_SIZE;

	makecontext( temp->context, wrapper, 1, temp );

	//insert tcb onto the heap/arraylist
	#ifdef RR
	// use queue for RR
	enqueue( thread_pool, temp );

	#endif	// RR

	#ifdef PSJF


	#endif	// PSJF
	// use min heap for PSJF

	return 0;
};

/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield()
{
	// YOUR CODE HERE
	
	// change current thread's state from Running to Ready
	// save context of this thread to its thread control block
	// switch from this thread's context to the scheduler's context
	kill( getpid(), SIGVTALRM );

	return 0;
};

/* terminate a thread */
/*
void mypthread_exit(void *value_ptr)
{
	// YOUR CODE HERE

	// preserve the return value pointer if not NULL
	// deallocate any dynamic memory allocated when starting this thread
	
	return;
};
*/


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr)
{
	// YOUR CODE HERE

	// wait for a specific thread to terminate
	// deallocate any dynamic memory created by the joining thread
	#ifdef RR
	for ( tcb *temp = thread_pool->head; temp->next != NULL; temp = temp->next )
	{
		if ( temp->tcb->tid = thread )
		{
			while ( temp->tcb->status != finished );
			void** return_value = &temp->tcb->return_val;
			free( temp->tcb->context );
			temp->prev->next = temp->next;
			free( temp->tcb );
			free( temp );
		}
	}


	#endif 	// RR

	#ifdef PSJF



	#endif 	// PSJF






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

	// turn off the timer to prevent scheduler being called inside scheduler. 
	timer = ( struct itimerval ) { 0 };
	setitimer( ITIMER_VIRTUAL, &timer, NULL );


	#ifdef RR
	sched_RR();
	#endif 	// RR
	
	#ifdef PSJF
	sched_PSJF();
	#endif 	// PSJF

	return;
}

/* Round Robin scheduling algorithm */
static void sched_RR()
{
	#ifdef RR
	// YOUR CODE HERE
	
	// Your own implementation of RR
	// (feel free to modify arguments and return types)

	free( scheduler_context.uc_stack.ss_sp );
	scheduler_context.uc_stack.ss_sp = malloc( STACK_SIZE );
	makecontext( &scheduler_context, schedule, 1, NULL );

	enqueue( thread_pool, curr_tcb );
	tcb* temp = curr_tcb;
	curr_tcb = dequeue( thread_pool );

	timer = ( struct itimerval )
	{
		.it_value.tv_usec 	= QUANTUM,
	};
	setitimer( ITIMER_VIRTUAL, &timer, NULL );


	swapcontext( temp->context, curr_tcb->context );
	
	#endif //RR
	return;
}

/* Preemptive PSJF (STCF) scheduling algorithm */
static void sched_PSJF()
{
	// YOUR CODE HERE

	// Your own implementation of PSJF (STCF)
	// (feel free to modify arguments and return types)
	free( scheduler_context.uc_stack.ss_sp );
	scheduler_context.uc_stack.ss_sp = malloc( STACK_SIZE );
	makecontext( &scheduler_context, schedule, 1, NULL );




	timer = ( struct itimerval )
	{
		.it_value.tv_usec 	= QUANTUM,
	};
	setitimer( ITIMER_VIRTUAL, &timer, NULL );


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
