#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <rtdk.h>

/*
 * Добавьте в код программы семафор. Используя семафор измените
 * ход выполнения программы — заблокируйте доступ задаче TaskOne
 * к глобальной переменной global до тех пор, пока не выполнится задача TaskTwo.
 * Выполните компилирование и запустите программу.
 */

#define ITER 10

#define SEM_INIT 1 // Установка начального значения семафора
#define SEM_MODE S_FIFO // Установка режима работы семафора

RT_SEM sem_desc; // Объявление семафора

static RT_TASK t1;
static RT_TASK t2;

int global = 0;

void taskOne(void *arg) {
    int i;
    for (i=0; i < ITER; i++) {
        rt_printf("I am taskOne and global = %d................\n", ++global);
    }
}

void taskTwo(void *arg) {
    int i;
    for (i=0; i < ITER; i++) {
        rt_printf("I am taskTwo and global = %d----------------\n", --global);
    }
    rt_sem_v(&sem_desc); // Увеличение счетчика семафора
    rt_sem_broadcast (&sem_desc); // Общий сигнал (разблокирование всех задач)
}

int main(int argc, char* argv[]) {
    /* Perform auto-init of rt_print buffers if the task doesn't do so */
    rt_print_auto_init(1);
    /* Avoids memory swapping for this program */
    mlockall(MCL_CURRENT|MCL_FUTURE);

    /* create the two tasks */
    rt_task_create(&t1, "task1", 0, 1, 0);
    rt_task_create(&t2, "task2", 0, 1, 0);

    rt_sem_create(&sem_desc,"MySemaphore",SEM_INIT,SEM_MODE); // Создать семафор...

    rt_task_start(&t2, &taskTwo, 0);
    // Уменьшение счетчика семафора, если счетчик = 0 то включается блокировка процесса
    rt_sem_p(&sem_desc,TM_INFINITE);
    rt_task_start(&t1, &taskOne, 0);

    rt_sem_delete(&sem_desc); // Удаление семафора
    return 0;
}
