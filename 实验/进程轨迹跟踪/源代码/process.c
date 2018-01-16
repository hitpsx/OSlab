#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define HZ    100

void cpuio_bound(int last, int cpu_time, int io_time);

int main(int argc, char * argv[])
{
       pid_t Pid1;
       pid_t Pid2;
       pid_t Pid3;

       Pid1 = fork(); 
       if (Pid1 < 0) printf("error in fork!"); 
       else if (Pid1 == 0) 
            {
                 printf("child process 1:\n");
                 cpuio_bound(5, 2, 2);
            }

       Pid2 = fork();
       if (Pid2 < 0) printf("error in fork!"); 
       else if (Pid2 == 0) 
            {
                 printf("child process 2:\n");
                 cpuio_bound(5, 4, 0);
            }

       Pid3 = fork();
       if (Pid3 < 0) printf("error in fork!"); 
       else if (Pid3 == 0) 
            {
                 printf("child process 3:\n");
                 cpuio_bound(5, 0, 4);
            }

       printf("This process's Pid is %d\n", getpid());
       printf("Pid of child process 1 is %d\n", Pid1);
       printf("Pid of child process 2 is %d\n", Pid2);
       printf("Pid of child process 3 is %d\n", Pid3);
       wait(NULL);
       wait(NULL);
       wait(NULL);
       return 0;
}

/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time)
{
    struct tms start_time, current_time;
    clock_t utime, stime;
    int sleep_time;

    while (last > 0)
    {
        /* CPU Burst */
        times(&start_time);
        /* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
         * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
         * 加上很合理。*/
        do
        {
            times(&t_time);
            utime = current_time.tms_utime - start_time.tms_utime;
            stime = current_time.tms_stime - start_time.tms_stime;
        } while ( ( (utime + stime) / HZ )  < cpu_time );
        last -= cpu_time;

        if (last <= 0 )
            break;

        /* IO Burst */
        /* 用sleep(1)模拟1秒钟的I/O操作 */
        sleep_time=0;
        while (sleep_time < io_time)
        {
            sleep(1);
            sleep_time++;
        }
        last -= sleep_time;
    }
}

