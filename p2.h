    #include <stdio.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <wait.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <errno.h>
    #include <dirent.h>
    #include <sys/wait.h>
    #include <dirent.h>
    #include <limits.h>
    #include "getword.h"
    #define STDIN 0
    #define STDOUT 1
    #define MAXITEM 100 /* max number of words */
    void parse();
    int killpg();
    char *realpath(const char *path, char *resolved_path);
