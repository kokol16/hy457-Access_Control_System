#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
int main()
{
   munmap(NULL, 0);
   fork();
   // printf("hello word\n");
   read(0,NULL,0);
}