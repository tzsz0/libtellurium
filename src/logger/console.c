#include<logger.h>
#include<stdio.h>


[[deprecated]]
extern int
logger_console_err(void * v, char const * const text)
{
    return fprintf(stderr, "%s", text) == 0;
}

static bool
logger_console_out(void * * v, enum logger_level level, char * text)
{
    /* TODO print some levels to stderr */
    return fprintf(stdout, "%s", text) == 0;
}

static bool
console_open()
{   }

static bool
console_close()
{   }

logger_output_t
logger_console_default = {
    .open = console_open,
    .close = console_close,
    .write = logger_console_out
};





