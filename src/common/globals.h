/*
 * global variables.. initially for the current directory and
 * some files that we are reading from there.. (book, wac.ci)
 * and later for the logger and shit..
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#ifndef WIN32
#include <unistd.h>
#define get_current_dir getcwd
#else
#include <direct.h>
#define get_current_dir _getcwd
#endif

#include <stdio.h>  /* defines FILENAME_MAX */
#include <string.h>
#include "define.h"

static char current_path[FILENAME_MAX];

int init_globals() {
    if (!get_current_dir(current_path, sizeof(current_path))) {
        //error
        return -1;
    }
    return 0;
}

#endif /* GLOBALS_H_ */
