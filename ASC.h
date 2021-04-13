#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h> /* for struct user_regs_struct */
#include <sys/ptrace.h>
#include <ctype.h>
#include <pthread.h>
#define SYS_CALLS_SIZE 356



typedef struct sys_call_info
{
    char *name;
    int id;
    int times;
} sys_call_info;

typedef struct sys_call_sequence
{
    char *name;
    int id;
} sys_call_sequence_array;

long sys_calls_count[SYS_CALLS_SIZE] ;

unsigned int system_calls_size(FILE *fp, unsigned int *sequence_size);

sys_call_sequence_array create_sys_call_list_entry(char *name);
sys_call_info create_sys_call_info_entry(char *_name, int times);
void print_sys_calls_array_info(sys_call_info *array, unsigned int size);
unsigned int fill_structures(FILE *fp, sys_call_info *sys_call_info_array, sys_call_sequence_array *seq_array);

sys_call_sequence_array *init_sys_call_table();

void reset_sys_calls_reset_counters(long *sys_calls_count);