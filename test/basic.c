#include<logger.h>
#include<logger/file.h>
#include<logger/console.h>
#include<stdlib.h>

int main()
{
    logger_opts_t defaults = {
        .linewidth = 80,
        .numoutputs = 2,
        .outputs = &(logger_output_t[]){logger_file_default, logger_console_default}
    };
    logger_t * log = logger_get("a new start", &defaults);
    logger_set_prefix(log, "basic");
    logger_info(log, "This works nicely! :)\n");

    logger_close_all();

    return EXIT_SUCCESS;
}

