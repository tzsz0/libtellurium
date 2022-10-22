#pragma once

#include<threads.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdatomic.h>
#include<logger/logger.h>


enum logger_level {
    LOGGER_NO_LEVEL,
    LOGGER_INFO,
    LOGGER_WARN,
    LOGGER_ERROR,
    LOGGER_FATAL,
    LOGGER_LEVEL_MAX
};



struct logger_opts {
    /* maximum number of characters per line
     * after that, wrap around */
    size_t linewidth;

    struct logger_target * targets;
};


struct logger {
    char * identifier;

    mtx_t lock;

    struct {
        /* double linked list */
        struct logger * * loggers;
        size_t num;
    } parents, children;

    struct logger_opts opts;

    struct {
        /* working data for each thread,
         * cannot be thread_local because
         * we have to set running=false
         * at some point */
        _Atomic bool volatile running;
    } threadinfo;
};


struct logger_output {

    bool colors;

    void * workingdata;

    bool (*open)(void * *);
    bool (*write)(void * *, enum logger_level, char *);
    bool (*close)(void * *);

};


typedef struct logger logger_t;
typedef struct logger_opts logger_opts_t;
typedef struct logger_output logger_output_t;


/* TODO this *will* break ABI if logger_opts_t is ever changed, should provide alternative interface someday in the future */
[[nodiscard]]
extern logger_t *   logger_get(char const * const, logger_opts_t const * const);


extern bool         logger_add_output(logger_t * const, logger_output_t const);


extern void         logger_write(logger_t * const, enum logger_level, char const * const, ...);


extern void         logger_vwrite(logger_t * const, enum logger_level, char const * const,  va_list);

extern void         logger_info(logger_t * const, char const * const, ...);


extern void         logger_warn(logger_t * const, char const * const, ...);


extern void         logger_error(logger_t * const, char const * const, ...);


extern void         logger_fatal(logger_t * const, char const * const, ...);


extern void         logger_close(logger_t * const);


extern void         logger_close_all();


extern void         logger_set_default_opts(logger_opts_t const [const static 1]);


extern size_t       logger_num_targets(logger_t const * const);


