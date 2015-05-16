#ifndef __THREAD_H__
#define __THREAD_H__

#ifndef USE_PTHREAD
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#ifndef NDEBUG
    #include <valgrind/valgrind.h>
#endif
#include "queue.h"

#define UNUSED(x) (void)(x)
#define STACK_SIZE 64*1024
#define TARGET_LATENCY 20000000 // 20 ms

/* Wrapper around a context function.
 * This structure holds necessary informations to retrieve
 * a thread exit value with its function returned.
 */
struct wrap {
    void *(*func)(void *);
    void *funcarg;
    void **retval;
};

/* Thread.
*/
typedef struct thread {
    ucontext_t *context;
    void *retval;
    void **retpointer;
    bool has_returned;
    int valgrind_stackid;
    struct wrap *wrap;
    struct thread *wanted_by;
    int nice;
    TAILQ_ENTRY(thread) threads;
} thread;

/* List of threads.
 * Holds informations for the sceduler.
 */
struct list_threads {
    thread *running;
    thread *main;
    ucontext_t *help_context;
    int help_context_stackid;
    int nice_total;
    TAILQ_HEAD(head, thread) head;
};

/* Adjust current thread priority
 */
extern void set_priority(int nice);

/* These functions are respectively the constructor and the destructor
 * which manage the thread list and the thread pool creation.
 */
static void thread_init() __attribute__ ((constructor));
static void thread_quit() __attribute__ ((destructor));

/* Create a new help context on the thread list `list`.
 * You're responsible for freeing it with `free_help_context`.
 */
void new_help_context(struct list_threads *list);

/* Free a help context on the thread list `list`.
 */
void new_help_context(struct list_threads *list);

/* Swap to the help context on the list `list` to free `to_free`,
 * save the current context to `previous` and set the `next` context.
 */
void swapcontext_free(struct list_threads *list, ucontext_t *previous, ucontext_t *next, thread *to_free);

/* Swap to the help context on the list `list` to free `to_free`,
 * and set the `next` context.
 */
void setcontext_free(struct list_threads *list, ucontext_t *next, thread *to_free);

/* Used as a context function to free in the help_context.
 */
void * free_func(void *to_free);

/* Initialize a context.
 * `uc` must be allocated.
 */
void init_context(ucontext_t *uc, void *stack, int size);

/* Wrap function to manage thread soft returns.
*/
void wrap(void *arg);

/* Initialize an allocated wrapper.
 * These are used as arguments to the `wrap` function.
 */
void init_wrap(struct wrap *w, void *(*func)(void *), void *funcarg);

/* Create and initialize a new thread.
 * You're responsible for freeing it with `free_thread_single_alloc`.
 */
thread * new_thread_single_alloc();

/* Free a thread.
*/
void free_thread_single_alloc();

/* Create and initialize a new thread list.
 * You're responsible for freeing it with `free_thread_list`.
 */
struct list_threads * new_thread_list();

/* Free a thread list.
*/
void free_thread_list(struct list_threads *list);

/* This struct tracks declared threads.
*/
extern struct list_threads *T;

/* Thread identifier.
*/
typedef void * thread_t;

/* Returns the current thread identifier.
*/
extern thread_t thread_self(void);

/* Create a new thread `newthread` which will execute `func` applied to `funcarg`.
 * Returns -1 if the creation is unsuccessful,
 * 0 otherwise.
 */
extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg);

/* Yield the execution to another thread
*/
extern int thread_yield(void);

/* Wait for a thread `thread` to return and place the returned value
 * at the address `*retval`.
 * If `retval` is NULL, the return value is ignored.
 */
extern int thread_join(thread_t thread, void **retval);

/* Terminate the current thread with the return value `retval`.
*/
extern void thread_exit(void *retval) __attribute__ ((__noreturn__));

#else /* USE_PTHREAD */

#include <sched.h>
#include <pthread.h>
#define thread_t pthread_t
#define thread_self pthread_self
#define thread_create(th, func, arg) pthread_create(th, NULL, func, arg)
#define thread_yield sched_yield
#define thread_join pthread_join
#define thread_exit pthread_exit

#endif /* USE_PTHREAD */

#endif /* __THREAD_H__ */
