#include "ASC.h"


unsigned int system_calls_zize(FILE *fp)
{
    unsigned int sys_calls_array_size = 0;
    int ch;
    if (fp == NULL)
    {
        fprintf(stderr, "error can't open file\n");
        return -1;
    }
    while ((ch = fgetc(fp)) != EOF)
    {

        if (ch == '\n')
        {
            sys_calls_array_size++;
        }
    }
    fseek(fp, 0, SEEK_SET);
    return sys_calls_array_size;
}

short fill_sys_calls_array(FILE * fp,sys_call_info * sys_calls_array )
{
    return 1;
}
int main(int argc, char **argv)
{
    FILE *fp;
    unsigned int sys_calls_array_size = 0;
    sys_call_info *sys_calls_array;
    if (argc > 1)
    {

        fp = fopen(argv[1], "r");
        sys_calls_array_size = system_calls_zize(fp);
        fprintf(stdout, "found :  %u sys calls\n", sys_calls_array_size);
        sys_calls_array = malloc(sizeof(sys_call_info) * sys_calls_array_size);
    
    
    }
    else
    {
        fprintf(stderr, "error no file inserted!\n");
        return -1;
    }
}