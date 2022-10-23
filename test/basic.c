#include<logger.h>
#include<logger/file.h>
#include<stdlib.h>

int main()
{
    logger_opts_t defaults = {
        .linewidth = 80,
        .numoutputs = 1,
        .outputs = &logger_file_default
    };
    logger_t * log = logger_get("a new start", &defaults);
    logger_set_prefix(log, "basic");
    logger_info(log, "Hello World!\n");

    logger_close_all();

    return EXIT_SUCCESS;
}

