#include<logger.h>
#include<stdio.h>
#ifdef __unix__
    #include<unistd.h>
#else
    #include<io.h>
#endif


static bool
file_exists(char * filename)
{
#ifdef __unix__
    return access(filename, W_OK) == 0;
#else
    return _access(filename, F_OK) ==0;
#endif
}

static bool
file_open(void * * data)
{
    char * fname = "file.log";
    if(file_exists(fname)) {
        /* noop for now */
    }
    *(FILE * *)data = fopen(fname, "a"); /* TODO pass filename to open handler */
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



