#include "thread.h"

#include <stdio.h>
#include <assert.h>
#include <time.h>

#define LOOP 200000

static void * threadfunc(void * arg) {
    char *name = arg;
    int i = 0;
#ifndef USE_PTHREAD
    if (strcmp(name, "thread1") == 0) {
        set_priority(1);
    }
#endif
    for (i = 0; i < LOOP; i++) {
        fprintf(stderr, "%s\n", name);
        /*nanosleep((struct timespec[]){{0, 100000000}}, NULL);*/
    }
    thread_exit(arg);
}

int main(int argc, char *argv[]) {
    thread_t thread1, thread2;
    void *retval1, *retval2;
    int err;

    printf("le main lance 2 threads...\n");
    err = thread_create(&thread1, threadfunc, "thread1");
    assert(!err);
    err = thread_create(&thread2, threadfunc, "thread2");
    assert(!err);
    printf("le main a lancé les threads %p et %p\n",
            (void*) thread1, (void*) thread2);

    printf("le main attend les threads\n");
    err = thread_join(thread1, &retval1);
    assert(!err);
    err = thread_join(thread2, &retval2);
    assert(!err);
    printf("les threads ont terminé en renvoyant '%s' and '%s'\n",
            (char *) retval1, (char *) retval2);

    return 0;
}
