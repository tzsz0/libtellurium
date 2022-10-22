#include<logger.h>
#include<stdio.h>



static bool
file_open(void * * data)
{
    *(FILE * *)data = fopen("file.log", "a"); /* TODO pass filename to open handler */
    return *data != NULL;
}


static bool
file_write(void * * v, enum logger_level level, char * text)
{
    fprintf(*v, "%s", text);
}

static bool
file_close(void * * v)
{
    fclose(*v);
}


logger_output_t
logger_file_default = {
    .open = file_open,
    .write = file_write,
    .close = file_close
};



