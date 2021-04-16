#include "ASC.h"

pthread_mutex_t lock;
pthread_t tid;
short int triggered = 1;


/**
 * @brief This method changes output color to red
 * 
 */
void print_Red()
{
  fprintf(stderr, "\033[0;31m");
}
/**
 * @brief This method reset output color from red to normal
 * 
 */
void reset_Red()
{
  fprintf(stderr, "\033[0m");
}
int sys_call_name_to_id(char *name)
{
    static short first_time = 1;
    static sys_call_sequence_array *sys_call_table;
    if (first_time)
    {
        sys_call_table = init_sys_call_table();
        first_time = 0;
    }
    unsigned int index = 0;
    for (index = 0; index < SYS_CALLS_SIZE; index++)
    {
        if (strcmp(name, sys_call_table[index].name) == 0)
        {
            return sys_call_table[index].id;
        }
    }
}
char *sys_call_id_to_name(unsigned int id)
{
    static short first_time = 1;
    static sys_call_sequence_array *sys_call_table;
    if (first_time)
    {
        sys_call_table = init_sys_call_table();
        first_time = 0;
    }
    unsigned int index = 0;
    for (index = 0; index < SYS_CALLS_SIZE; index++)
    {
        if (id == sys_call_table[index].id)
        {
            return sys_call_table[index].name;
        }
    }
}
sys_call_sequence_array *init_sys_call_table()
{
    sys_call_sequence_array *sys_call_table;
    FILE *fp = NULL;
    char *line, *sys_call_id;
    short flag = 0;
    size_t size = 1024, index = 0;
    unsigned int name_size = 100, digit_size = 100;
    char delim[] = "#define __NR_";
    unsigned int delim_index = 0, line_index = 0;
    line = malloc(sizeof(char) * size);
    fp = fopen("/usr/include/i386-linux-gnu/asm/unistd_32.h", "r");
    //fp = fopen("unistd_32.h", "r");

    if (fp == NULL)
    {
        fprintf(stderr, "error can't open file \n");
        return NULL;
    }
    sys_call_table = malloc(sizeof(sys_call_sequence_array) * SYS_CALLS_SIZE);
    memset(sys_call_table, 0, SYS_CALLS_SIZE);
    while (getline(&line, &size, fp) != EOF)
    {
        //name_size = 20;
        //digit_size = 20;
        line_index = 0;
        delim_index = 0;
        flag = 0;

        while (line[line_index] != '\0' && line_index < 13)
        {
            if (line[line_index] == ' ')
            {
                if (line_index == 0)
                {
                    flag = 1;
                    break;
                }
            }
            //printf("%c",line[line_index]);
            //printf("%c\n",delim[delim_index]);

            if (line[line_index] != delim[delim_index])
            {
                //printf("lala2\n");
                flag = 1;
                break;
            }
            line_index++;
            delim_index++;
        }
        if (flag)
        {
            //printf("lala3\n");

            continue;
        }
        else
        {
            int index_str = 0;
            int digit_index = 0;
            sys_call_table[index].name = malloc(sizeof(char) * name_size);
            sys_call_id = malloc(sizeof(char) * digit_size);

            while (line[line_index] != ' ')
            {
                memcpy(&sys_call_table[index].name[index_str], &line[line_index], sizeof(char));
                // printf("lala\n");
                if (index_str >= name_size)
                {
                    //printf("lala : index_str %u\n", index_str);
                    name_size *= 2;
                    //printf("lala : new size  %u\n", name_size);
                    sys_call_table[index].name = (char *)realloc(sys_call_table[index].name, name_size * sizeof(char));

                    //printf("lala2\n");
                }
                line_index++;
                index_str++;
            }
            while (line[line_index] != '\0')
            {
                if (isdigit(line[line_index]))
                {
                    memcpy(&sys_call_id[digit_index++], &line[line_index], sizeof(char));
                    //         printf("%c ", line[line_index]);
                }
                if (digit_index >= digit_size)
                {
                    digit_size *= 2;
                    sys_call_id = realloc(sys_call_id, digit_size * sizeof(char));
                }
                line_index++;
            }
            sys_call_table[index].name[index_str] = '\0';
            sys_call_id[digit_index] = '\0';
            // printf("%s\n", sys_call_id);
            sys_call_table[index].id = atoi(sys_call_id);
            // printf("%s ", sys_call_table[index].name);
            // printf("%d\n", sys_call_table[index].id);

            index++;
        }
    }

    return sys_call_table;
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
    entry.id = sys_call_name_to_id(entry.name);
    return entry;
}
sys_call_sequence_array create_sys_call_seq_entry(char *_name)
{
    sys_call_sequence_array entry;
    int str_len = strlen(_name);
    entry.name = malloc(str_len + 1);
    memcpy(entry.name, _name, (strlen(_name) + 1));
    entry.id = sys_call_name_to_id(entry.name);
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
            else
            {
                sys_call_name[index] = '\0';
                //fprintf(stdout, "%s\n", sys_call_name);
                seq_array[seq_array_index++] = create_sys_call_seq_entry(sys_call_name);

                index = 0;
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
void *threadproc(void *sys_calls_count_array)
{

    //printf("xaxa\n");
    while (triggered)
    {
        sleep(1);

        //printf("reset called\n");+
        //lock
        pthread_mutex_lock(&lock);
        reset_sys_calls_reset_counters(sys_calls_count);
        pthread_mutex_unlock(&lock);
    }
}
void reset_sys_calls_reset_counters(long *sys_calls_count)
{
    unsigned int index = 0;
    for (index = 0; index < SYS_CALLS_SIZE; index++)
    {

        sys_calls_count[index] = 0;
    }
}
void access_control_system(sys_call_info *sys_call_info_array, unsigned int size, long sys_call_id)
{
    unsigned int index = 0;
    static int first_time = 1;
    if (first_time)
    {
        //create the thread that reset sys_calls counters
        pthread_create(&tid, NULL, &threadproc, NULL);
        first_time = 0;
    }

    //lock
    pthread_mutex_lock(&lock);
    sys_calls_count[sys_call_id] = sys_calls_count[sys_call_id] + 1;
    //printf("count : %ld\n", sys_calls_count[47]);

    //printf("id : %ld\n", sys_call_id);

    for (index = 0; index < size; index++)
    {
        if (sys_calls_count[sys_call_info_array[index].id] >= sys_call_info_array[index].times)
        {
            print_Red();
            fprintf(stderr,"Warning programm called :  %s %u times in 1 second!!\n",  sys_call_info_array[index].name , sys_call_info_array[index].times);
            reset_Red();
            
            sys_calls_count[sys_call_info_array[index].id] = 0;
        }
    }

    //unlock
    pthread_mutex_unlock(&lock);

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

            int status;
            short is_acm_active = 0;
            unsigned track = 0;
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
                            printf("program  made a system call %ld (%s) \n", orig_eax, sys_call_id_to_name(orig_eax));
                            if (!is_acm_active)
                            {

                                if (seq_array[track].id == orig_eax)
                                    track++;
                                else
                                {
                                    track = 0;
                                    if (seq_array[track].id == orig_eax)
                                        track++;
                                }
                            }

                            if (track == sys_calls_seq_size || is_acm_active) //trigger
                            {
                                triggered = 1;
                                access_control_system(sys_call_info_array, sys_calls_array_size, orig_eax);
                                is_acm_active = 1;
                                track = 0;
                            }
                            // orig_eax = ptrace(PTRACE_PEEKUSER, pid, 4 * ORIG_EAX, NULL);
                            // printf("The child made a system call %ld\n", orig_eax);
                            is_first_time = 0;
                        }
                        is_first_time++;
                    }
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {
                        // child has exited or terminated
                        //if (is_acm_active)
                        triggered = 0;
                        pthread_join(tid, NULL);
                        pthread_mutex_destroy(&lock);

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