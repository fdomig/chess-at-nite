/*
 * Written by Gabor Egressy
 * gabor@vmunix.com
 * Original    : January 1995
 * Last update : November 1997
 *
 * this is unixio.h : must be included with unixio.c if you wish to use any
 * of the functionality of unixio.c
 *
 * If you want to use the curses routines make sure you #define USE_CURSES
 * before the line #ifdef USE_CURSES in this file
 *
 * Make sure to link in termcap/terminfo library
 * You might need to change the line
 * #include <termcap.h> to #include <terminfo.h> in the unixio.c file
 */

#ifndef UNIXIO_H
#define UNIXIO_H

/*
 * timeout used by select; you might need to increase this on a slow machine
 * or over a slow modem line; it was tested on a local machine
 * I wrote this on a 486DX2 66Mhz machine running FreeBSD
 */
#define TIMEOUT_SEC       0 /* seconds */
#define TIMEOUT_USEC      0 /* micro seconds */

/*
 * error occurred; query errno for the error may not be fatal error
 * although it most likely is
 */
#define IOERROR  (-1)

/*
 * no input received; for doing non blocking I/O
 */
#define NO_INPUT    1100

/*
 * returned on extended chars; cursor keys and the 8 function keys
 * should return their own constants if your terminal supports them if
 * EXTENDED is returned then you can check the structure passed to
 * getkey for the actual key sequence, as long as you passed a non-null
 * structure pointer
 */
#define EXTENDED    1000

/*
 * structure to hold the extended keys, if num > 1 keys[0] will equal
 * the escape key(0x1b) and keys[1] to keys[num-1] will be the other
 * characters sent by a particular key eg. the up cursor sends ^[[A that
 * is (escape,[,A), most of the time
 */
struct extended {
    char keys[32]; /* stores the characters sent by key */
    int num;       /* number of characters sent by key  */
};

/*
 * constants for the cursor keys, function keys, some of the keypad keys
 * - returned only if defined for the terminal
 */
#define CURSOR_UP        300
#define CURSOR_DOWN      301
#define CURSOR_RIGHT     302
#define CURSOR_LEFT      303
#define FUNCTION_1       400
#define FUNCTION_2       401
#define FUNCTION_3       402
#define FUNCTION_4       403
#define FUNCTION_5       404
#define FUNCTION_6       405
#define FUNCTION_7       406
#define FUNCTION_8       407
#define FUNCTION_9       408
#define FUNCTION_10      409
#define FUNCTION_11      410
#define FUNCTION_12      411
#define PAGE_NEXT        500
#define PAGE_PREV        501
#define HOME_KEY         502
#define INSERT_KEY       503
#define END_KEY          504

int termSetup(void);

/*
 * allows non-blocking input from terminal;
 * getkey() does not wait for input but returns immediately;
 * value returned is NO_INPUT in that case 
 */
#define NON_BLOCKING  0
#define BLOCKING      1

/*
 * getkey can use cbreak or raw mode for input; in raw mode the program
 * cannot be killed off with control-C and some other characters get
 * turned off as well, such as control-Z, control-\
 */
#define RAW_MODE     1
#define CBREAK_MODE  0

/* pointer to extended keys can be the null pointer */
int getkey(int no_block,int raw,struct extended *keys);

/* macro for those who only want basic functionality */
#define getKey() getkey(BLOCKING,CBREAK_MODE,0)

/* pointer to input can be the null pointer */
int kbdhit(int *input);

/*
 * see warning about these functions just above their function headers
 * these routines perform a similar function as the <curses> routines
 */
int Raw(void);
int noRaw(void);
int Cbreak(void);
int noCbreak(void);
int Echo(void);
int noEcho(void);

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* THE FOLLOWING FUNCTIONS ARE MEANT SPECIFICALLY FOR CURSES */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef USE_CURSES

#include <curses.h>

int wgetkey(WINDOW *win);
int wgetline(WINDOW *win,char *buf,int size);
int wgettoken(WINDOW *win,char *buf,int size,char *toklim);

#endif /* USE_CURSES */

#endif /* UNIXIO_H */
