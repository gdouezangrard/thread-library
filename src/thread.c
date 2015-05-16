#include "thread.h"

struct list_threads *T = NULL;

static struct sigevent sev;
static struct itimerspec its;
static timer_t timerid;
int enablestatus = 0;

static void set_timer_preempt(long long time_slice) {
    its.it_value.tv_sec = time_slice / 1000000000;
    its.it_value.tv_nsec = time_slice % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    timer_settime(timerid, 0, &its, NULL);
}

void preempt(int signum) {
    if (signum == SIGALRM && enablestatus) {
        set_timer_preempt(T->running->nice*TARGET_LATENCY/T->nice_total);
#ifndef NDEBUG
        printf("PREEMPT %d\n", T->running->nice*TARGET_LATENCY/T->nice_total);
#endif
        thread_yield();
    }
}

extern void set_priority(int nice) {
    if (T != NULL && T->running != NULL) {
        assert(nice >= 1);
        assert(nice <= 20);
        T->nice_total -= T->running->nice;
        T->running->nice = nice;
        T->nice_total += T->running->nice;
    }
}

void thread_preemption_init() {
    struct sigaction new_s;
    new_s.sa_handler = preempt;
    sigemptyset(&new_s.sa_mask);
    new_s.sa_flags = 0;
    sigaction(SIGALRM, &new_s, NULL);

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create(CLOCK_THREAD_CPUTIME_ID, &sev, &timerid);

    set_timer_preempt(T->running->nice*TARGET_LATENCY/T->nice_total);
}

void thread_preemption_quit() {
    timer_delete(timerid);
}

void thread_preemption_disable() {
    enablestatus = false;
}

void thread_preemption_enable() {
    enablestatus = true;
}

void new_help_context(struct list_threads *list) {
    if (list != NULL) {
        list->help_context = (ucontext_t *)malloc(sizeof(ucontext_t));
        void *stack = malloc(STACK_SIZE);
        init_context(list->help_context, stack, STACK_SIZE);

#ifndef NDEBUG
        list->help_context_stackid = VALGRIND_STACK_REGISTER(list->help_context->uc_stack.ss_sp,
                list->help_context->uc_stack.ss_sp + list->help_context->uc_stack.ss_size);
#endif
    }
}

void free_help_context(struct list_threads *list) {
    if (T != NULL) {
#ifndef NDEBUG
        VALGRIND_STACK_DEREGISTER(list->help_context_stackid);
#endif
        free(list->help_context->uc_stack.ss_sp);
        free(list->help_context);
    }
}

void init_context(ucontext_t *uc, void *stack, int size) {
    getcontext(uc);
    uc->uc_stack.ss_size = size;
    uc->uc_stack.ss_sp = stack;
    uc->uc_link = NULL;
}

void wrap(void *arg) {
    struct wrap *argument = (struct wrap *)arg;
    thread_preemption_enable();
    thread_exit(argument->func(argument->funcarg));
}

void init_wrap(struct wrap *w, void *(*func)(void *), void *funcarg) {
    w->func = func;
    w->funcarg = funcarg;
}

thread * new_thread_single_alloc() {
    char *memory = (char *)malloc(sizeof(thread) + sizeof(struct wrap) + sizeof(ucontext_t) + STACK_SIZE);

    thread *t = (thread *)memory;
    memset(t, 0, sizeof(thread));

    t->wrap = (struct wrap *)((char *)t + sizeof(thread));

    t->context = (ucontext_t *)((char *)t->wrap + sizeof(struct wrap));
    void *stack = (void *)((char *)t->context + sizeof(ucontext_t));
    init_context(t->context, stack, STACK_SIZE);

#ifndef NDEBUG
    t->valgrind_stackid = VALGRIND_STACK_REGISTER(t->context->uc_stack.ss_sp,
            t->context->uc_stack.ss_sp + t->context->uc_stack.ss_size);
#endif

    t->nice = 10;
    T->nice_total += t->nice;
    return t;
}

void free_thread_single_alloc(thread *t) {
#ifndef NDEBUG
    VALGRIND_STACK_DEREGISTER(t->valgrind_stackid);
#endif
    if (t == T->main) {
        T->main = NULL;
    }
    T->nice_total -= t->nice;
    free(t);
    t = NULL;
}

struct list_threads * new_thread_list() {
    struct list_threads * list = (struct list_threads *)malloc(sizeof(struct list_threads));
    TAILQ_INIT(&list->head);
    new_help_context(list);
    return list;
}

void * free_func(void *to_free) {
    free_thread_single_alloc((thread *)to_free);
    return NULL;
}

void swapcontext_free(struct list_threads *list, ucontext_t *previous, ucontext_t *next, thread *to_free) {
    list->help_context->uc_link = next;
    makecontext(list->help_context, (void (*)(void))free_func, 1, to_free);

    swapcontext(previous, list->help_context);
    thread_preemption_enable();
}

void setcontext_free(struct list_threads *list, ucontext_t *next, thread *to_free) {
    list->help_context->uc_link = next;
    makecontext(list->help_context, (void (*)(void))free_func, 1, to_free);

    setcontext(list->help_context);
}

void free_thread_list(struct list_threads *list) {
    free_help_context(list);
    free(list);
    list = NULL;
}

extern thread_t thread_self() {
    return T->running;
}

extern int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg) {
    thread_preemption_disable();

    thread *nt = new_thread_single_alloc();
    *newthread = (thread_t *)nt;

    init_wrap(nt->wrap, func, funcarg);
    makecontext(nt->context, (void (*)(void))wrap, 1, nt->wrap);

    TAILQ_INSERT_TAIL(&T->head, nt, threads);

    thread_preemption_enable();
    return 0;
}

extern int thread_yield(void) {
    thread_preemption_disable();

    thread *previous = T->running;

    TAILQ_INSERT_TAIL(&T->head, previous, threads);
    T->running = TAILQ_FIRST(&T->head);
    TAILQ_REMOVE(&T->head, T->running, threads);

    swapcontext(previous->context, T->running->context);
    thread_preemption_enable();

    return 0;
}

extern int thread_join(thread_t threadt, void **retval) {
    thread_preemption_disable();
    thread *joined = (thread *)threadt;
    thread *owner = T->running;

    joined->retpointer = retval;
    if (joined->has_returned == true) {
        if (retval != NULL) {
            *retval = joined->retval;
        }

        TAILQ_INSERT_TAIL(&T->head, owner, threads);
    } else {
        joined->wanted_by = owner;
    }

    thread *next = TAILQ_FIRST(&T->head);
    if (next != NULL) {
        T->running = next;
        TAILQ_REMOVE(&T->head, T->running, threads);

        if (joined->has_returned == true) {
            swapcontext_free(T, owner->context, T->running->context, joined);
        } else {
            swapcontext(owner->context, T->running->context);
        }
    }

    thread_preemption_enable();
    return 0;
}

extern void thread_exit(void *retval) {
    thread_preemption_disable();

    thread *previous = T->running;
    bool must_free = false;

    if (previous->wanted_by != NULL) {
        TAILQ_INSERT_TAIL(&T->head, previous->wanted_by, threads);
    }

    thread *next = TAILQ_FIRST(&T->head);

    previous->retval = retval;
    previous->has_returned = true;
    if (previous->retpointer != NULL) {
        *previous->retpointer = previous->retval;

        if (next != NULL) {
            must_free = true;
        }
    }

    if (next != NULL) {
        T->running = next;
        TAILQ_REMOVE(&T->head, T->running, threads);

        if (must_free) {
            setcontext_free(T, T->running->context, previous);
        } else {
            setcontext(T->running->context);
        }
    }

    setcontext_free(T, NULL, previous);

    exit(0);
}

static void thread_init() {
    T = new_thread_list();

    thread *main_thread = new_thread_single_alloc();

    T->running = main_thread;
    T->main = main_thread;

    thread_preemption_init();
    thread_preemption_enable();
}

static void thread_quit() {
    thread_preemption_disable();
    if (T != NULL && T->main != NULL) {
        free_thread_single_alloc(T->main);
    }
    if (T != NULL) {
        free_thread_list(T);
    }
    thread_preemption_quit();
}
