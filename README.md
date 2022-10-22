# libtellurium
C11 library for comfy logging


### TODOs

 - [ ] Basic single-threaded logging
   - [ ] Local files
   - [ ] Terminal
   - [ ] Syslogd
   - [ ] Custom
 - [ ] Custom format strings
 - [ ] Custom format entries
 - [ ] Stable API (and ABI?)
 - [ ] Use intel/safestringlib
 - [ ] Custom Loggers
 - [ ] Thread-safe
 - [ ] Non-blocking (atomic queue?)
 - [ ] Use atexit(??)
 - [ ] Colors
 - [ ] Max line width
 - [ ] Prefixes


## Easy to use and robust API


```c
logger_t * logger = logger_get("myapp", NULL);
logger_info(logger, "I'd like to interject for a moment...");
...
logger_close(logger);
or
logger_close_all();
```



