#include<logger.h>
#include<stddef.h>
#include<safe_lib.h>
#include<stdbool.h>
#include<threads.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>


/* we hand out pointers to logger_t via our API.
 * The objects behind those pointers may not be
 * moved during operation. So once we return a pointer
 * it has to be valid until the logger is closed.
 * However, we still have to provide a dynamic
 * array for all loggers.
 * To implement this, loggers points to a array of
 * logger_t pointers. This array can ideed move in
 * memory but the objects behind those pointers
 * stay where they are */


/*
 * **loggers
 * ┌──────────┬─────────┬─────────┬─────────┬─────────┬───────────────────────┐
 * │ptr 0     │ ptr 1   │ ptr 2   │ ptr 3   │ ptr 4   │ more...               │
 * │          │         │         │         │         │                       │
 * └────┬─────┴───────┬─┴────┬────┴─────────┴─────────┴───────────────────────┘
 *      │             │      │
 *      │             │      └──────┐
 *      │             │             │
 * ┌────▼───────┐  ┌──▼─────────┐  ┌▼───────────┐
 * │            │  │            │  │            │
 * │ logger_t   │  │ logger_t   │  │ logger_t   │
 * │            │  │            │  │            │
 * │            │  │            │  │            │
 * └────────────┘  └────────────┘  └────────────┘
 */


static size_t           num_loggers = 0;
static logger_t * *     loggers     = NULL;
static mtx_t            loggers_lock;
static once_flag        co_flag = ONCE_FLAG_INIT;



static char * *     split_line_at(char const * const, size_t);


static logger_opts_t defaults = {
    /* TODO */
    .linewidth = 80,
    /* TODO get proper defaults */
    .numoutputs = 1,
    .outputs = (logger_output_t[]) {
        { .colors=true,
        },
    }
};

static void
init_lock() { mtx_init(&loggers_lock, mtx_plain | mtx_recursive); }


static logger_t *
logger_find(char const * const id)
{
    for(size_t i = 0; i < num_loggers; i++) {
        /* TODO use safe version */
        if(strcmp(loggers[i]->identifier, id) == 0) {
            return loggers[i];
        }
    }
    return NULL;
}


static void
logger_init(logger_t * restrict const logger, size_t id_length, char const id[const id_length])
{
    /* TODO use safe version */
    /* make sure this is freed */
    logger->identifier = calloc(id_length + 1, sizeof(char));
    if(logger->identifier != NULL) {
        strcpy(logger->identifier, id);
        mtx_init(&logger->lock, mtx_plain | mtx_recursive);
    } else {
        return;
    }
}

static bool
logger_add(logger_t * log)
{
    /* TODO check if already added */
    /* TODO add lock */
    if(logger_find(log->identifier) != NULL) {
        return true;
    } else {
        logger_t * * tmp = realloc(loggers, num_loggers + 1);
        if(tmp == NULL)
            return false;
        loggers = tmp;
        num_loggers += 1;

        loggers[num_loggers - 1] = log;

        return true;
    }
}

static void
logger_remove(logger_t * log)
{
    /* TODO add lock */
    bool found = false;
    for(size_t i = 0; i < num_loggers; i++) {
        if(loggers[i] == log) {
            /* swap with last element so we can just resize the array e.g. delete the last element */
            loggers[i] = loggers[num_loggers - 1];
            num_loggers -= 1;
            found = true;
            break;
        }
    }
    if(found) {
        /* TODO C2X change of realloc, take care */
        logger_t * * new = realloc(loggers, num_loggers);
        if(new != NULL)
            loggers = new;
        if(num_loggers == 0) {
            loggers = NULL;
        }
    }
    /* TODO when num_loggers==0, free(loggers); and loggers=NULL; */
    return;
}


void
logger_close(logger_t * const logger)
{
    /* TODO, perhaps acquire lock before deletion */
    /* TODO close ouputs */
    free(logger->identifier);
    mtx_destroy(&logger->lock);

    for(size_t i = 0; i < logger->opts.numoutputs; i++) {
        logger->opts.outputs[i].close(&logger->opts.outputs[i].workingdata);
    }

    /* TODO remove connection with children and parents */

    logger_remove(logger);

    free(logger);
}

void logger_close_all()
{
    /* this does not work */
    //for(size_t i = 0; i < num_loggers; i++) {
    //    logger_close(loggers[i]);
    //}
    mtx_lock(&loggers_lock);
    while(num_loggers > 0)
        logger_close(loggers[0]);
    mtx_unlock(&loggers_lock);
}


logger_t *
logger_get(char const * const id, logger_opts_t const * const opts)
{
    mtx_lock(&loggers_lock);
    logger_t * ret = logger_find(id);
    if(ret == NULL) {
        ret = calloc(1, sizeof *ret);
        if(ret != NULL) {
            logger_init(ret, 0, id);
            /* TODO use opts */
            memcpy(&ret->opts, opts, sizeof ret->opts);
            /* TODO move this somewhere else */
            for(size_t i = 0; i < opts->numoutputs; i++) {
                opts->outputs[i].open(&opts->outputs[i].workingdata);
            }
        }
    }
    mtx_unlock(&loggers_lock);
    return ret;
}

void
logger_set_default_opts(logger_opts_t const opts[const static 1]) {
    defaults = *opts;
}

size_t
logger_num_targets(logger_t const logger[const static 1])
{
    return 0;
}


logger_opts_t const *
logger_opts_defaults()
{
    return &defaults;
}


static bool
logger_is_prefix(logger_t const parent[const static 1],
                 logger_t const child[const static 1])
{
    return false;
}

static bool
logger_link(logger_t * const child, logger_t * const parent)
{
    logger_t * * newparentslist = realloc(child->parents.loggers, (child->parents.num + 1) * sizeof *newparentslist);
    logger_t * * newchildrenlist = realloc(parent->children.loggers, (parent->children.num + 1) * sizeof *newchildrenlist);

    if(newparentslist) {
        child->parents.loggers = newparentslist;
    }
    if(newchildrenlist) {
        parent->children.loggers = newchildrenlist;
    }


    if(newparentslist && newchildrenlist) {
        
        newparentslist[child->parents.num++] = parent;
        newchildrenlist[parent->children.num++] = child;

        return true;
    } else {
        return false;
    }
}



bool
logger_set_prefix(logger_t logger[const static 1], char const * const newprefix)
{
    char * copied = strdup(newprefix);
    if(copied != NULL) {
        logger->prefix = copied;
        return true;
    } else {
        return false;
    }
}

char const *
logger_get_prefix(logger_t const * const logger)
{
    return strdup(logger->prefix);
}


static char *
format_time(char const * const formatstr)
{
    /* strftime ffs does *NOT* return the lengths it *WOULD* need if called with NULL,0
     * this is pissing me off bc I now have to estimate the number of chars needed
     * for that reason we might make a preprocessor macro out of this. */
    size_t const MAX_TIMESTAMP_SIZE = 100;
    struct timespec now;
    timespec_get(&now, TIME_UTC); /* TODO adjust for local time */

    /* TODO move this out of here so not every logger call has to allocate this */
    char * timebuff = calloc(MAX_TIMESTAMP_SIZE, sizeof *timebuff);

    if(timebuff != NULL) {
        strftime(timebuff, MAX_TIMESTAMP_SIZE, formatstr, gmtime(&now.tv_sec));
        return timebuff;
    } else {
        return NULL;
    }
}

void
logger_write(logger_t * const logger, enum logger_level level, char const * const msg, ...)
{
    va_list args;
    va_start(args, msg);
    logger_vwrite(logger, level, msg, args);
    va_end(args);
}

void
logger_vwrite(logger_t * const logger, enum logger_level level, char const * const msg, va_list args)
{
    /* TODO reenable line splitting after some testing */
    //char * * substr = split_line_at(msg, logger->opts.linewidth);

    /* broadcast all messages to all parent loggers */
    //for(size_t i = 0; i < logger->parents.num; i++) {
    //    logger_vwrite(logger->parents.loggers + i, level, msg, args);
    //}

    size_t const len = vsnprintf(NULL, 0, msg, args) + 1;
    char line[len]; /* TODO this should be moved to the heap later on */
    memset(line, '\0', len);
    vsnprintf(line, len, msg, args);

    char * timestr = format_time("%D %T");

    size_t lendate = snprintf(NULL, 0, "[%s] %s\n", timestr, line);
    char withdate[lendate];
    snprintf(withdate, lendate, "[%s] %s\n", timestr, line);

    free(timestr);
    
    for(size_t i = 0; i < logger->opts.numoutputs; i++) {
        logger_output_t out = logger->opts.outputs[i];
        out.write(&out.workingdata, level, withdate);
    }
    
}


static
char * *
split_line_at(char const * const line, size_t maxlength)
{
    /* TODO check this for string correctness */
    size_t numsubstr = 0; /* number of maxlength-sized strings we need to split line */
    size_t lenstr = strlen(line);

    while(numsubstr * maxlength < lenstr)
        numsubstr++;

    char * * substr = calloc(numsubstr, sizeof *substr);

    if(substr == NULL)
        return NULL;

    for(size_t i = 0; i < numsubstr; i++) {
        /* TODO use safe version */
        substr[i] = calloc(maxlength + 1, sizeof *substr[i]);
        strncpy(substr[i], line + i * maxlength, maxlength);
    }

    return substr;
}


void
logger_info(logger_t * const logger, char const * const msg, ...)
{
    va_list args;
    va_start(args, msg);
    logger_vwrite(logger, LOGGER_INFO, msg, args);
    va_end(args);
}


