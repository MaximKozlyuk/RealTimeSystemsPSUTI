#include <stdio.h> 
#include <signal.h> 
#include <unistd.h> 
#include <sys/mman.h>
#include <native/task.h> 
#include <native/timer.h>

RT_TASK demo_task;

void demo(void *arg) {
	RTIME now, previous;
	rt_task_set_periodic(NULL, TM_NOW, 500000000); 
	previous = rt_timer_read();
	while (1) { 
		rt_task_wait_period(NULL); 
		now = rt_timer_read();
		printf(
			"Time since last turn: %ld.%06ld ms\n",
			(long)(now - previous) / 1000000, 
			(long)(now - previous) % 1000000
		); 
		previous = now;
	} 
}

void catch_signal(int sig) {

}

int main(int argc, char* argv[]) {
	signal(SIGTERM, catch_signal); 
	signal(SIGINT, catch_signal);
	mlockall(MCL_CURRENT|MCL_FUTURE);
	rt_task_create(&demo_task, "trivial", 0, 99, 0);

	rt_task_start(&demo_task, &demo, NULL);
	pause();
	rt_task_delete(&demo_task); 
}
