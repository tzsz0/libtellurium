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
    logger_info(log, "Hello World!\n");

    return EXIT_SUCCESS;
}

