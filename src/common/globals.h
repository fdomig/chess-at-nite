/*
 * This file is part of the chess-at-nite project [chess-at-nite.googlecode.com]
 *
 * Copyright (c) 2009-2010 by
 *   Franziskus Domig
 *   Panayiotis Lipiridis
 *   Radoslav Petrik
 *   Thai Gia Tuong
 *
 * For the full copyright and license information, please visit:
 *   http://chess-at-nite.googlecode.com/svn/trunk/doc/LICENSE
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

static char current_path[FILENAME_MAX];

int init_globals() {
    if (!get_current_dir(current_path, sizeof(current_path))) {
        //error
        return -1;
    }
    return 0;
}

#endif /* GLOBALS_H_ */
