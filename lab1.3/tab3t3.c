#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>

#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>

#include <rtdk.h>

#define ITER 10

#define SEM_INIT 0
#define SEM_MODE S_FIFO

RT_SEM sem1;
RT_SEM sem2;

static RT_TASK t1;
static RT_TASK t2;
static RT_TASK t3;

int global = 0;

void taskOne(void *arg) {
    rt_sem_p(&sem1, TM_INFINITE);

    int i;
    for (i = 0; i < ITER; i++) {
        rt_printf("I am taskOne and global=%d 1 1 1\n", ++global);
    }
}

void taskTwo(void *arg) {
    rt_sem_p(&sem2, TM_INFINITE);

    int i;
    for (i = 0; i < ITER; i++) {
        rt_printf("I am taskTwo and global=%d 2 2 2\n", -global);
    }

    rt_sem_v(&sem1);
}

void taskThree(void *arg) {
    int i;
    for (i = 0; i < ITER; i++) {
        rt_printf("I am taskThree and global=%d 3 3 3\n", ++global);
    }
    rt_sem_v(&sem2);
}

int main(int argc, char *argv[]) {
    rt_print_auto_init(1);

    mlockall(MCL_CURRENT | MCL_FUTURE);

    rt_sem_create(&sem1, "MySemaphore", SEM_INIT, SEM_MODE);
    rt_sem_create(&sem2, "Sem2", SEM_INIT, SEM_MODE);

    rt_task_create(&t1, "task1", 0, 1, 0);
    rt_task_create(&t2, "task2", 0, 1, 0);
    rt_task_create(&t3, "task3", 0, 1, 0);

    rt_task_start(&t1, &taskOne, 0);
    rt_task_start(&t2, &taskTwo, 0);
    rt_task_start(&t3, &taskThree, 0);
    return 0;
}
