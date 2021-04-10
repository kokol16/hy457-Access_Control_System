#include <stdlib.h>
#include <stdio.h>

typedef struct sys_call_info
{
    char * name ;
    unsigned int id;
    unsigned int times;
}sys_call_info;

typedef struct sys_call_sequence_list
{
    char * name ;
    struct sys_call_list *next;
}sys_call_sequence_list;


unsigned int system_calls_zize(FILE *fp);