#pragma once

#include<threads.h>
#include<stdarg.h>
#include<stdbool.h>


enum logger_level {
    LOGGER_NO_LEVEL,
    LOGGER_INFO,
    LOGGER_WARN,
    LOGGER_ERROR,
    LOGGER_FATAL,
};


struct logger {
    void * v;
};

struct logger_opts {
    void * v;
};

struct logger_output {

};


typedef struct logger logger_t;
typedef struct logger_opts logger_opts_t;
typedef struct logger_output logger_output_t;


[[nodiscard]]
extern logger_t *   logger_get(char const * const, logger_opts const * const);


extern bool         logger_add_output(logger_t * const, logger_output_t const);


extern void         logger_write(logger_t * const, enum logger_level, char const * const, ...);


extern bool         logger_vwrite(logger_t * const, enum logger_level, va_list);


extern void         logger_info(logger_t * const, char const * const, ...);


extern void         logger_warn(logger_t * const, char const * const, ...);


extern void         logger_error(logger_t * const, char const * const, ...);


extern void         logger_fatal(logger_t * const, char const * const, ...);


extern void         logger_close(logger_t * const);
