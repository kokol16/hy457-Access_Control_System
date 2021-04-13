#include "ASC.h"

#define __NR_restart_syscall 0
#define __NR_exit 1
#define __NR_fork 2
#define __NR_read 3
#define __NR_write 4
#define __NR_open 5
#define __NR_close 6
#define __NR_waitpid 7
#define __NR_creat 8
#define __NR_link 9
unsigned int sys_call_name_to_id(char *name)
{
    if (strcmp(name, "restart_syscall") == 0)
        return __NR_restart_syscall;
    else if (strcmp(name, "exit") == 0)
}
void print_sys_calls_sequence_array(sys_call_sequence_array *array, unsigned int size)
{
    unsigned int index = 0;
    while (index < size)
    {
        fprintf(stdout, "name : %s ", array[index].name);
        fprintf(stdout, " id : %u\n", array[index].id);
        index++;
    }
}
void print_sys_calls_array_info(sys_call_info *array, unsigned int size)
{
    unsigned int index = 0;
    while (index < size)
    {
        fprintf(stdout, "name : %s ", array[index].name);
        fprintf(stdout, " id : %u ", array[index].id);
        fprintf(stdout, " times  : %d\n", array[index].times);
        index++;
    }
}
sys_call_info create_sys_call_info_entry(char *_name, int times)
{
    sys_call_info entry;
    int str_len = strlen(_name);
    entry.name = malloc(str_len + 1);
    memcpy(entry.name, _name, str_len + 1);
    entry.times = times;
    entry.id = 0;

    return entry;
}
sys_call_sequence_array create_sys_call_seq_entry(char *_name)
{
    sys_call_sequence_array entry;
    int str_len = strlen(_name);
    entry.name = malloc(str_len + 1);
    memcpy(entry.name, _name, (strlen(_name) + 1));
    entry.id = 0;
    return entry;
}

unsigned int fill_structures(FILE *fp, sys_call_info *sys_call_info_array, sys_call_sequence_array *seq_array)
{
    int ch = 0;
    short is_info = 0;
    char *sys_call_name;
    unsigned int index = 0, str_size = 5, sys_call_info_array_index = 0, tmp_index = 0, seq_array_index = 0;
    unsigned int times_start_pos = 0;
    sys_call_name = malloc(sizeof(char) * str_size);

    if (fp == NULL)
    {
        fprintf(stderr, "error can't open file\n");
        return 0;
    }
    while (1)
    {
        if (ch == EOF)
            break;
        ch = fgetc(fp);
        if (ch == '\n' || ch == EOF)
        {
            sys_call_name[index] = '\0';

            if (is_info)
            {

                while (sys_call_name[tmp_index] != '\0')
                {
                    if (sys_call_name[tmp_index] == ' ')
                    {
                        times_start_pos = tmp_index + 1;
                        break;
                    }
                    tmp_index++;
                }
                sys_call_name[times_start_pos - 1] = '\0';
                sys_call_info_array[sys_call_info_array_index++] = create_sys_call_info_entry(sys_call_name, atoi(sys_call_name + times_start_pos));
            }

            tmp_index = 0;
            times_start_pos = 0;
            index = 0;
            is_info = 1;
            continue;
        }
        else
        {
            if (!is_info)
            {
                if (ch == ' ' || ch == '\n' || ch == '\r')
                {
                    sys_call_name[index] = '\0';
                    //fprintf(stdout, "%s\n", sys_call_name);
                    seq_array[seq_array_index++] = create_sys_call_seq_entry(sys_call_name);

                    index = 0;
                }
                else
                {
                    sys_call_name[index++] = ch;
                }
            }
            else
            {
                if (ch != '\r')
                    sys_call_name[index++] = ch;
            }
        }

        if (index >= str_size)
        {
            str_size *= 2;
            sys_call_name = realloc(sys_call_name, str_size * sizeof(char));
        }
    }

    fseek(fp, 0, SEEK_SET);
    return 1;
}

unsigned int system_calls_size(FILE *fp, unsigned int *sequence_size)
{
    unsigned int sys_calls_array_size = 0;
    int ch;
    short is_info = 0;

    if (fp == NULL)
    {
        fprintf(stderr, "error can't open file\n");
        return -1;
    }
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == ' ' && !is_info)
            (*sequence_size) = (*sequence_size) + 1;

        if (ch == '\n')
        {
            is_info = 1;
            sys_calls_array_size++;
        }
    }
    fseek(fp, 0, SEEK_SET);
    (*sequence_size) = (*sequence_size) + 1;
    return sys_calls_array_size;
}
void access_control_system(sys_call_info *sys_call_info_array, sys_call_sequence_array *seq_array)
{
}

short fill_sys_calls_array(FILE *fp, sys_call_info *sys_calls_array)
{
    return 1;
}
int main(int argc, char **argv)
{
    FILE *fp;
    sys_call_info *sys_call_info_array;
    sys_call_sequence_array *seq_array;

    unsigned int sys_calls_array_size = 0, sys_calls_seq_size = 0;
    sys_call_info *sys_calls_array;
    if (argc > 1)
    {

        fp = fopen(argv[1], "r");
        sys_calls_array_size = system_calls_size(fp, &sys_calls_seq_size);
        fprintf(stdout, "found :  %u sys calls\n", sys_calls_array_size);
        sys_call_info_array = malloc(sizeof(sys_call_info) * sys_calls_array_size);
        seq_array = malloc(sizeof(sys_call_sequence_array) * sys_calls_seq_size);
        fill_structures(fp, sys_call_info_array, seq_array);
        print_sys_calls_sequence_array(seq_array, sys_calls_seq_size);
        print_sys_calls_array_info(sys_call_info_array, sys_calls_array_size);

        if (argv[2] == NULL)
        {
            fprintf(stderr, "error not a second argument provided \n");
        }
        else
        {
            long sc_number, sc_retcode;
            int counter = 0;
            int in_call = 0;
            int status;
            long orig_eax;
            unsigned int is_first_time;
            struct user_regs_struct regs;
            int pid = fork();
            if (pid == 0)
            {
                char *args[2];
                int length = strlen(argv[2]);
                args[0] = malloc(sizeof(char) * (length + 1 + 2));
                args[1] = NULL;
                args[0][0] = '.';
                args[0][1] = '/';
                memcpy((args[0] + 2), argv[2], length + 1);
                if (ptrace(PTRACE_TRACEME, 0, NULL, NULL))
                    perror("ptrace");
                execvp(args[0], args);
            }
            else
            {
                is_first_time = 0;
                while (1)
                {

                    wait(&status);
                    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
                    {
                        if (is_first_time % 2 == 0)
                        {
                            orig_eax = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * ORIG_EAX, NULL);
                            printf("copy made a system call %ld\n", orig_eax);
                            /* orig_eax = ptrace(PTRACE_PEEKUSER, pid, 4 * ORIG_EAX, NULL);
                            printf("The child made a system call %ld\n", orig_eax);*/
                            is_first_time = 0;
                        }
                        is_first_time++;
                    }
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {
                        // child has exited or terminated
                        break;
                    }

                    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
                    //ptrace(PTRACE_CONT, pid,	 NULL, NULL);
                }
            }
        }
    }
    else
    {
        fprintf(stderr, "error no file inserted!\n");
        return -1;
    }
}