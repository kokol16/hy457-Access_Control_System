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
/**
 * @brief This method returns the id of he current
 * system call , using his name
 * 
 * @param name name of sys call
 * @return int id of sys call
 */
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
/**
 * @brief This method returns the name of
 * a sys call knowing his id 
 * 
 * @param id of syscall
 * @return char* name of sys call
 */
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
    return NULL;
}
/**
 * @brief This method is parsing the file with
 * the system calls id and returns an array
 * that has the name and the for each system call
 * 
 * @return sys_call_sequence_array* 
 */
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
    if (fp == NULL)
    {
        fprintf(stderr, "error can't open file \n");
        return NULL;
    }
    sys_call_table = malloc(sizeof(sys_call_sequence_array) * SYS_CALLS_SIZE);
    memset(sys_call_table, 0, SYS_CALLS_SIZE);
    while (getline(&line, &size, fp) != EOF)
    {
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

            if (line[line_index] != delim[delim_index])
            {
                flag = 1;
                break;
            }
            line_index++;
            delim_index++;
        }
        if (flag)
        {

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
                if (index_str >= name_size)
                {
                    name_size *= 2;
                    sys_call_table[index].name = (char *)realloc(sys_call_table[index].name, name_size * sizeof(char));
                }
                line_index++;
                index_str++;
            }
            while (line[line_index] != '\0')
            {
                if (isdigit(line[line_index]))
                {
                    memcpy(&sys_call_id[digit_index++], &line[line_index], sizeof(char));
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
            sys_call_table[index].id = atoi(sys_call_id);

            index++;
        }
    }

    return sys_call_table;
}

/**
 * @brief This method prints the array with the name and 
 * id of each syscall
 * 
 * @param array of syscalls 
 * @param size  of array
 */
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
/**
 * @brief This method prints the array that has each 
 * system call we track and the informations about
 * how many times can be called , his name and id
 * 
 * @param array of system call information
 * @param size  of array
 */
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

/**
 * @brief Create a sys call info entry object
 * 
 * @param _name name of system call
 * @param times how many times can be called each second
 * @return sys_call_info 
 */
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
/**
 * @brief Create a sys call seq entry object
 * Those system calls are the trigger the
 * access contol system
 * @param _name of syscall
 * @return sys_call_sequence_array 
 */
sys_call_sequence_array create_sys_call_seq_entry(char *_name)
{
    sys_call_sequence_array entry;
    int str_len = strlen(_name);
    entry.name = malloc(str_len + 1);
    memcpy(entry.name, _name, (strlen(_name) + 1));
    entry.id = sys_call_name_to_id(entry.name);
    return entry;
}

/**
 * @brief This method fills the array that keeps
 * the sequence that triggers the control access system.
 * and the array that holds the information about
 * how many times can a system call be called
 * 
 * @param fp input file 
 * @param sys_call_info_array 
 * @param seq_array 
 * @return unsigned int  (1)on success (0) on failure
 */
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

/**
 * @brief This method finds the size of the system
 * call sequence 
 * 
 * @param fp 
 * @param sequence_size 
 * @return unsigned int 
 */
unsigned int system_calls_size(FILE *fp, unsigned int *sequence_size)
{
    unsigned int sys_calls_array_size = 0;
    int ch;
    short is_info = 0;

    if (fp == NULL)
    {
        fprintf(stderr, "error can't open file\n");
        return 0;
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
/**
 * @brief This method resets the array that keeps
 * how many times each system call is called every
 * second 
 * 
 * @param sys_calls_count_array 
 * @return void* 
 */
void *threadproc(void *sys_calls_count_array)
{

    while (triggered)
    {
        sleep(1);

        //lock
        pthread_mutex_lock(&lock);
        reset_sys_calls_reset_counters(sys_calls_count);
        pthread_mutex_unlock(&lock);
    }
}
/**
 * @brief Reset the times that each system call
 * is called by tracee
 * 
 * @param sys_calls_count 
 */
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
    for (index = 0; index < size; index++)
    {
        if (sys_calls_count[sys_call_info_array[index].id] >= sys_call_info_array[index].times)
        {
            print_Red();
            fprintf(stderr, "Warning programm called :  %s %u times in 1 second!!\n", sys_call_info_array[index].name, sys_call_info_array[index].times);
            reset_Red();

            sys_calls_count[sys_call_info_array[index].id] = 0;
        }
    }

    //unlock
    pthread_mutex_unlock(&lock);
}
int access_control_system_wrapper(int argc, char **argv)
{
    FILE *fp;
    sys_call_info *sys_call_info_array;
    sys_call_sequence_array *seq_array;
    unsigned int sys_calls_array_size = 0, sys_calls_seq_size = 0;
    sys_call_info *sys_calls_array;
    if (argc > 1)
    {
        if (argv[1] == NULL)
            return ARGS_ERROR;
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            fprintf(stderr, "error can't open file \n");
            return FILE_ERROR;
        }
        sys_calls_array_size = system_calls_size(fp, &sys_calls_seq_size);
        fprintf(stdout, "found :  %u sys calls\n", sys_calls_array_size);
        sys_call_info_array = malloc(sizeof(sys_call_info) * sys_calls_array_size);
        seq_array = malloc(sizeof(sys_call_sequence_array) * sys_calls_seq_size);
        if (!fill_structures(fp, sys_call_info_array, seq_array))
        {
            return FILL_STRUCTURES_ERROR;
        }
        print_sys_calls_sequence_array(seq_array, sys_calls_seq_size);
        print_sys_calls_array_info(sys_call_info_array, sys_calls_array_size);
        if (argv[2] == NULL)
        {
            fprintf(stderr, "error not a second argument provided \n");
            return ARGS_ERROR;
        }
        else
        {

            int status;
            short is_acm_active = 0;
            unsigned track = 0;
            long orig_eax;
            unsigned int is_first_time = 0;
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
                    ptrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACESYSGOOD);
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {

                        // child has exited or terminated
                        triggered = 0;
                        pthread_join(tid, NULL);
                        pthread_mutex_destroy(&lock);
                        break;
                    }
                    if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
                    {

                        if (is_first_time % 2 == 0)
                        {
                            orig_eax = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * ORIG_EAX, NULL);
                            printf("program  made a system call %ld (%s) \n", orig_eax, sys_call_id_to_name(orig_eax));
                            if (!is_acm_active)
                            {

                                if (seq_array[track].id == orig_eax)
                                    ++track;
                                else
                                {
                                    track = 0;
                                    if (seq_array[track].id == orig_eax)
                                        ++track;
                                }
                            }

                            if (track == sys_calls_seq_size || is_acm_active) //trigger
                            {
                                triggered = 1;
                                access_control_system(sys_call_info_array, sys_calls_array_size, orig_eax);
                                is_acm_active = 1;
                                track = 0;
                            }
                            is_first_time = 0;
                        }
                        ++is_first_time;
                    }

                    ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
                }
            }
        }
    }
    else
    {
        fprintf(stderr, "error no file inserted!\n");
        return FILE_ERROR;
    }
    return SUCCESS;
}
int main(int argc, char **argv)
{
    int res = access_control_system_wrapper(argc, argv);
    if(res==SUCCESS)
    {
        printf("ALL WENT GOOD \n");
    }
    else 
    {
        printf("SOMETHING WENT WRONG \n");

    }
}