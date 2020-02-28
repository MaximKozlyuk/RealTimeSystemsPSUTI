#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <rtdk.h>

/*
 Создайте еще одну задачу — Task3 — аналогичную TaskOne и TaskTwo.
 Третья задача должна стартовать последней.
 Используя семафор, необходимо распределить доступ задач
 к переменой в таком порядке: Task3 > TaskTwo > TaskOne.
 */

#define ITER 10

#define SEM_INIT 1
#define SEM_MODE S_FIFO

RT_SEM sem_desc;

static RT_TASK t1;
static RT_TASK t2;
static RT_TASK t3;

int global = 0;

void endTask () {
    rt_sem_v(&sem_desc);
    rt_sem_broadcast (&sem_desc);
}

void taskOne(void *arg) {
    rt_sem_p(&sem_desc,TM_INFINITE);
    int i;
    for (i=0; i < ITER; i++) {
        rt_printf("I am taskOne and global = %d................\n", ++global);
    }
    endTask();
}

void taskTwo(void *arg) {
    rt_sem_p(&sem_desc,TM_INFINITE);
    int i;
    for (i=0; i < ITER; i++) {
        rt_printf("I am taskTwo and global = %d----------------\n", --global);
    }
    endTask();
}

void taskThree(void *arg) {
    int i;
    for (i=0; i < ITER; i++) {
        rt_printf("I am task3 and global = %d................\n", ++global);
    }
    endTask();
}

int main(int argc, char* argv[]) {

    rt_print_auto_init(1);
    mlockall(MCL_CURRENT|MCL_FUTURE);

    rt_task_create(&t1, "task1", 0, 1, 0);
    rt_task_create(&t2, "task2", 0, 1, 0);
    rt_task_create(&t3, "task3", 0, 1, 0);

    rt_sem_create(&sem_desc,"MySemaphore",SEM_INIT,SEM_MODE);

    rt_task_start(&t1, &taskOne, 0);
    rt_task_start(&t2, &taskTwo, 0);
    rt_task_start(&t3, &taskThree, 0);

    rt_sem_delete(&sem_desc);

    return 0;
}
