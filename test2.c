#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
int done = 0;
void *threadproc(void *arg);
int main()
{
    
}

void *threadproc(void *arg)
{
    while (!done)
    {
        sleep(1);
        printf("xaxa\n");
    }
    return 0;
}
