#include<threads.h> /* call_once */
#include<stdbool.h>
#include<syslog.h>
#include<logger.h>

static once_flag flag = ONCE_FLAG_INIT;
static _Atomic bool volatile initialized = false;
static mtx_t counter_lock;

static size_t refs = 0;

static void init_mutex() { mtx_init(&counter_lock, mtx_plain); initialized = true; }

static bool
open_syslogd()
{
#if 0
    /* TODO potential race cond, fixed by busy wait? */
    call_once(&flag, init_mutex);
    while(!initialized); /* getting a bit hacky, getting a bit frisky */
    mtx_lock(&counter_lock);
    if(refs++ == 0) {
        /* open here */
    }
    mtx_unlock(&counter_lock);
#endif
}

static bool
close_syslogd()
{
#if 0
    mtx_lock(&counter_lock);
    if(--refs == 0) {
        /* actually close syslog */
    }
    mtx_unlock(&counter_lock);
#endif
}

static bool
write_syslogd(void * * data, enum logger_level level, char * message)
{
    static int const levels[LOGGER_LEVEL_MAX] = {
        [LOGGER_NO_LEVEL] = LOG_INFO,
        [LOGGER_INFO] = LOG_INFO,
        [LOGGER_WARN] = LOG_WARNING,
        [LOGGER_ERROR] = LOG_ERR,
        [LOGGER_FATAL] = LOG_EMERG,
    };

    syslog(levels[level], message);
}


logger_output_t
logger_syslog_default = {
    .open = open_syslogd,
    .write = write_syslogd,
    .close = close_syslogd
};




