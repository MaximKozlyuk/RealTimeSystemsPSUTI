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

static RT_TASK t1;
static RT_TASK t2;
static RT_TASK t3;

int global = 0;

/**
 * rt_sem_p - Уменьшение счетчика семафора, если счетчик = 0 то включается блокировка процесса
 * rt_sem_v(&sem_desc); - Увеличение счетчика семафора
 */

void taskOne(void *arg) {
    rt_sem_p(&sem1, TM_INFINITE);
    int i;
    for (i = 0; i < ITER; i++) {
        rt_printf("I am taskOne and global=%d\n", ++global);
    }
}

void taskTwo(void *arg) {
    rt_sem_p(&sem1, TM_INFINITE);
    int i;
    for (i = 0; i < ITER; i++) {
        rt_printf("I am taskTwo and global=%d\n", -global);
    }
    rt_sem_v(&sem1);
}

void taskThree(void *arg) {
    int i;
    for (i = 0; i < ITER; i++) {
        rt_printf("I am taskThree and global=%d\n", ++global);
    }
    rt_sem_v(&sem1);
}

int main(int argc, char *argv[]) {
    rt_print_auto_init(1);

    mlockall(MCL_CURRENT | MCL_FUTURE);

    rt_sem_create(&sem1, "MySemaphore", SEM_INIT, SEM_MODE);

    rt_task_create(&t1, "task1", 0, 1, 0);
    rt_task_create(&t2, "task2", 0, 1, 0);
    rt_task_create(&t3, "task3", 0, 1, 0);

    rt_task_start(&t1, &taskOne, 0);
    rt_task_start(&t2, &taskTwo, 0);
    rt_task_start(&t3, &taskThree, 0);
    return 0;
}
