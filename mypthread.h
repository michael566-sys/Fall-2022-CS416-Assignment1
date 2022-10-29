// File:	mypthread_t.h

// List all group members' names:
// iLab machine tested on:

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* in order to use the built-in Linux pthread library as a control for benchmarking, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ucontext.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

typedef uint mypthread_t;

	/* add important states in a thread control block */

typedef enum thread_status
{
    waiting, running, ready, finished
}thread_status;

typedef struct threadControlBlock
{
	// YOUR CODE HERE	
	
	// thread Id
	// thread status
	// thread context
	// thread stack
	// thread priority
	// And more ...

    mypthread_t     tid;                //thread id
    thread_status   status;             //running, waiting, finish
    ucontext_t      context;            //ucontext_t
    uint8_t         priority;           //priority: lower is better, 0 means not ready
    void*           (*func)(void*);     //function pointer
    void*           args;               //arguments
    void*           return_val;         //return value

} tcb;


// wait node used in mutex
typedef struct queue_node{
    tcb *tcb;
    struct queue_node *next;
} queue_node;


typedef struct queue{
    queue_node *head;
    queue_node *tail;
    int size;
} queue;


/* mutex struct definition */
typedef struct mypthread_mutex_t
{
	// YOUR CODE HERE
    int     status;     //1: currently holding the lock, 0: not holding the lock
    tcb     *owner;      //current owner of the mutex
    queue   *wait_queue; // threads that in the blocking/waiting list

} mypthread_mutex_t;


typedef struct min_heap
{
    int capacity;       //capacity of the mean heap
    int size;           //current heap_size
    tcb *tcb;
}min_heap;


//would like to use macros, but macros don't work with ctypes
static inline int left_child(int index);            //get left child index
static inline int right_child(int index);           //get right child index
static inline int parent(int index);                //get parent index

min_heap heap_init(int capacity);                   //init min heap (insert the initial capacity)
static inline void swap(tcb *v1, tcb *v2);          //swap nodes(tcb)
void heapify(min_heap *heap, int index);            //heapify
void insert(min_heap *heap, tcb data);           //insert vertex into the heap, heapify will be called automatically
tcb pop_first(min_heap *heap);                   

queue* queue_init();
void enqueue (queue* queue, tcb *tcb ); 
tcb* dequeue (queue* queue );

/* Function Declarations: */
void scheduler( int sig );

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg);

/* current thread voluntarily surrenders its remaining runtime for other threads to use */
int mypthread_yield();

/* terminate a thread */
void mypthread_exit(void *value_ptr);

/* wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr);

/* initialize a mutex */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

/* aquire a mutex (lock) */
int mypthread_mutex_lock(mypthread_mutex_t *mutex);

/* release a mutex (unlock) */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);

/* destroy a mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex);

#ifdef USE_MYTHREAD
#define pthread_t mypthread_t
#define pthread_mutex_t mypthread_mutex_t
#define pthread_create mypthread_create
#define pthread_exit mypthread_exit
#define pthread_join mypthread_join
#define pthread_mutex_init mypthread_mutex_init
#define pthread_mutex_lock mypthread_mutex_lock
#define pthread_mutex_unlock mypthread_mutex_unlock
#define pthread_mutex_destroy mypthread_mutex_destroy
#endif

#endif
