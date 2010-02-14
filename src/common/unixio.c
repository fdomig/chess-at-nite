/*
 * Written by Gabor Egressy 
 * gabor@vmunix.com
 * Original    : January 1995
 * Last update : November 1997
 *
 * The functions contained herein have been tested on FreeBSD and LINUX.
 * The operating system used must be POSIX compliant UNIX to utilize
 * getkey() and kbdhit(). you might need to include some of the if lines
 * on some compilers with broken selects and headers. eg. on hp-ux you
 * might need to change #if 0 to #if 1
 *
 * all of the functions in this file return the constant IOERROR when an
 * error occurs; make sure to check for this return value.
 * make sure you include unixio.h in necessary headers
 *
 * Make sure to link in termcap/terminfo library
 * You might need to change the line
 * #include <termcap.h> to #include <terminfo.h> in the unixio.c file
 */

#include "unixio.h"

#include <stdlib.h>
#include <ctype.h>
#include <termcap.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

#if 0
typedef int fd_set;
#endif

#if 0
struct timeval {
    long tv_sec;
    long tv_usec;
};
#endif

#if 0
#define _INCLUDE_POSIX_SOURCE
#endif

#include <unistd.h>

#define _POSIX_SOURCE
#define _POSIX_C_SOURCE

#include <signal.h>
#include <termios.h>

static struct termios start_attr, save_attr, orig_attr;
static struct sigaction oact;
static enum {g_RAW, g_CBREAK, g_RESET} tty_status = g_RESET;
static int first = 1,no_block = 0;
static struct {
    char up[32], down[32], left[32], right[32], f1[32], f2[32], f3[32], f4[32],
         f5[32], f6[32], f7[32], f8[32], f9[32], f10[32], f11[32], f12[32],
         page_next[32], page_prev[32], home_key[32], insert_key[32], end_key[32],
         clear_screen[32];
} ext_keys;

/* 
 * signal handling functions; these are not interface functions -
 * termSetup() is - do not call these functions
 */
void saveMode(int);
void resetMode(int);
void dieOnSignal(int);
void cleanup(void);

static int timeout(void);
static int extended_keys(void);

#define ESC 0X1B

/*
 * getkey :
 *  
 * returns a character read from the keyboard : including the cursor keys on
 * ansi terminals - on non-ansi terminals compare the entry in keys to
 * termcap/terminfo entries this can also be done for the function keys
 *
 * it's a good idea to turn off echo yourself as it gives a better
 * interface than letting getkey try and turn it off
 * when using curses make sure to turn off echo, if you want the character
 * typed to be echoed to the window just use wgetkey() instead of getkey();
 * if you are not using curses and you want the character echoed after you
 * turned off echo just use the following code >
 * int input;
 *  
 * if(isprint((input = getkey(NON_BLOCKING,NULL))))
 *	 fputc(input,stdout);
 *
 * you can also use this function to do non-blocking i/o
 * the first parameter to getkey determines the type of input you want
 *
 * the input is also saved in the structure <struct extended> along with the
 * number of characters sent by the key if the second parameter is not 0 (NULL)
 *
 * if your terminal defines them then the constants for the cursor keys
 * and the 8 function keys are also returned
 *
 * MAKE SURE YOU USE AN INTEGER TO READ CHARACTER INTO
 * THIS FUNCTION MAY NOT WORK ON A TELNET SESSION TRY TO ADJUST THE VALUE
 * OF THE TIMEOUT TO SOMETHING LIKE A SECOND OR MORE
 *
 * IN  : constant to decide on blocking or non-blocking i/o
 *       pointer to structure that will contain character sequence sent by a
 *       key and the number of characters sent by that key; this parameter
 *       can be null
 * OUT : a character or a constant depending on the key pressed
 *       regular keys and control keys are returned as is
 *       cursor keys are returned as a constant on ansi terminals
 *       if keys is not null the sequence of characters is also stored there
 */
int
getkey(int block,int raw,struct extended *e_keys)
{
	unsigned char ch,keys[32];
	int n, i, retval = EXTENDED;
	struct termios oattr, attr;


    if(tty_status == g_RESET || raw) {
        if((tcgetattr)(STDIN_FILENO,&oattr) < 0)
            return IOERROR;
        attr = oattr;
        if(raw) {
            attr.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
            attr.c_iflag &= ~(BRKINT | ICRNL | INLCR | INPCK | ISTRIP | IXON);
            attr.c_oflag &= ~OPOST;
            attr.c_cflag &= ~(CSIZE | PARENB);
            attr.c_cflag |= CS8;
        }
        else
            attr.c_lflag &= ~(ICANON | ECHO);
        attr.c_cc[VMIN] = 1;
        attr.c_cc[VTIME] = 0;
        if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&attr) < 0)
            return IOERROR;
    } else if(!(isatty)(STDIN_FILENO))
        return IOERROR;

    no_block = !block;
	if(no_block) {
		if((n = timeout()) < 0) {
			retval = IOERROR;
            goto END;
        }
		if(!n)
			retval = NO_INPUT;
	}

    if(first > 0) {
        if(extended_keys() == IOERROR)
            first = IOERROR;
        else
            first = 0;
    }

	if(retval == EXTENDED) {
		for(i = 0,n = 1;n;++i) {
			if((read)(STDIN_FILENO,&ch,1) != 1) {
				retval = IOERROR;
                goto END;
            }
            keys[i] = ch;
			if((n = timeout()) < 0) {
				retval = IOERROR;
                goto END;
            }
		}
        keys[i] = 0;
		if(e_keys) {
			e_keys->num = i;
			strcpy(e_keys->keys,keys);
		}
		if(i == 1 || i > 1 && keys[0] != ESC)
            retval = keys[0];
		else if(first != IOERROR) {
            if(!strcmp(keys,ext_keys.up) || !strcmp(keys,"\x1b[A"))
                retval = CURSOR_UP;
            else if(!strcmp(keys,ext_keys.down) || !strcmp(keys,"\x1b[B"))
                retval = CURSOR_DOWN;
            else if(!strcmp(keys,ext_keys.right) || !strcmp(keys,"\x1b[C"))
                retval = CURSOR_RIGHT;
            else if(!strcmp(keys,ext_keys.left) || !strcmp(keys,"\x1b[D"))
                retval = CURSOR_LEFT;
            else if(!strcmp(keys,ext_keys.f1))
                retval = FUNCTION_1;
            else if(!strcmp(keys,ext_keys.f2))
                retval = FUNCTION_2;
            else if(!strcmp(keys,ext_keys.f3))
                retval = FUNCTION_3;
            else if(!strcmp(keys,ext_keys.f4))
                retval = FUNCTION_4;
            else if(!strcmp(keys,ext_keys.f5))
                retval = FUNCTION_5;
            else if(!strcmp(keys,ext_keys.f6))
                retval = FUNCTION_6;
            else if(!strcmp(keys,ext_keys.f7))
                retval = FUNCTION_7;
            else if(!strcmp(keys,ext_keys.f8))
                retval = FUNCTION_8;
            else if(!strcmp(keys,ext_keys.f9))
                retval = FUNCTION_9;
            else if(!strcmp(keys,ext_keys.f10))
                retval = FUNCTION_10;
            else if(!strcmp(keys,ext_keys.f11))
                retval = FUNCTION_11;
            else if(!strcmp(keys,ext_keys.f12))
                retval = FUNCTION_12;
            else if(!strcmp(keys,ext_keys.page_next))
                retval = PAGE_NEXT;
            else if(!strcmp(keys,ext_keys.page_prev))
                retval = PAGE_PREV;
            else if(!strcmp(keys,ext_keys.insert_key))
                retval = INSERT_KEY;
            else if(!strcmp(keys,ext_keys.home_key))
                retval = HOME_KEY;
            else if(!strcmp(keys,ext_keys.end_key))
                retval = END_KEY;
		}
	}
END:
    if(tty_status == g_RESET || raw)
        if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&oattr) < 0)
            return IOERROR;

	return retval;
}

/*
 * kbdhit :
 * return non-zero if a key on the keyboard was hit; 0 otherwise
 * the key that was typed will be stored in input iff it is a valid character
 * otherwise -1 is stored in input: valid characters are those that fit
 * into one char; if input is null nothing is stored in input
 * might be a good idea to turn off echo when using this function; you can 
 * turn echo back on when you need to
 * IN  : pointer to variable to store input
 * OUT : non-zero if keyboard was hit, 0 if not; IOERROR on error
 */
int
kbdhit(int *input)
{
	unsigned char ch;
	int n, retval;
	struct termios oattr, attr;


    if(tty_status == g_RESET) {
        if((tcgetattr)(STDIN_FILENO,&oattr) < 0)
            return IOERROR;
        attr = oattr;
        attr.c_lflag &= ~(ICANON | ECHO);
        attr.c_cc[VMIN] = 1;
        attr.c_cc[VTIME] = 0;
        if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&attr) < 0)
            return IOERROR;
    } else if(!(isatty)(STDIN_FILENO))
        return IOERROR;

    no_block = 1;
	if((n = timeout()) < 0)
		return IOERROR;
    retval = n;
	if(retval) { 
		if((read)(STDIN_FILENO,&ch,1) != 1)
			return IOERROR;
        if(input)
            *input = ch;
		if(ch == ESC) {
			if((n = timeout()) < 0)
				return IOERROR;
			if(n) {
				do {
					if((read)(STDIN_FILENO,&ch,1) != 1)
						return IOERROR;
					if((n = timeout()) < 0)
						return IOERROR;
				} while(n);
				if(input)
					*input = (-1);
			}
		}
	}
    if(tty_status == g_RESET)
        if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&oattr) < 0)
            return IOERROR;

	return retval;
}

/*
 * get some of the extended keys from the termcap database
 */
static int
extended_keys(void)
{
    char termcap[1024],*term,*cap,*ret;


    memset(&ext_keys,0,sizeof ext_keys);
    if((cap = malloc(1024)) == NULL)
        return IOERROR;
    if((term = (getenv)("TERM")) == NULL) {
        if(tgetent(termcap,"ansi") < 1)
            return IOERROR;
    }
    else {
        if((tgetent)(termcap,term) < 1)
            if((tgetent)(termcap,"ansi") < 1)
                return IOERROR;
    }

    if((ret = (tgetstr)("ku",&cap)) != NULL)
        strcpy(ext_keys.up,ret);
    if((ret = (tgetstr)("kd",&cap)) != NULL)
        strcpy(ext_keys.down,ret);
    if((ret = (tgetstr)("kr",&cap)) != NULL)
        strcpy(ext_keys.right,ret);
    if((ret = (tgetstr)("kl",&cap)) != NULL)
        strcpy(ext_keys.left,ret);
    if((ret = (tgetstr)("k1",&cap)) != NULL)
        strcpy(ext_keys.f1,ret);
    if((ret = (tgetstr)("k2",&cap)) != NULL)
        strcpy(ext_keys.f2,ret);
    if((ret = (tgetstr)("k3",&cap)) != NULL)
        strcpy(ext_keys.f3,ret);
    if((ret = (tgetstr)("k4",&cap)) != NULL)
        strcpy(ext_keys.f4,ret);
    if((ret = (tgetstr)("k5",&cap)) != NULL)
        strcpy(ext_keys.f5,ret);
    if((ret = (tgetstr)("k6",&cap)) != NULL)
        strcpy(ext_keys.f6,ret);
    if((ret = (tgetstr)("k7",&cap)) != NULL)
        strcpy(ext_keys.f7,ret);
    if((ret = (tgetstr)("k8",&cap)) != NULL)
        strcpy(ext_keys.f8,ret);
    if((ret = (tgetstr)("k9",&cap)) != NULL)
        strcpy(ext_keys.f9,ret);
    if((ret = (tgetstr)("k;",&cap)) != NULL)
        strcpy(ext_keys.f10,ret);
    if((ret = (tgetstr)("F1",&cap)) != NULL)
        strcpy(ext_keys.f11,ret);
    if((ret = (tgetstr)("F2",&cap)) != NULL)
        strcpy(ext_keys.f12,ret);
    if((ret = (tgetstr)("kN",&cap)) != NULL)
        strcpy(ext_keys.page_next,ret);
    if((ret = (tgetstr)("kP",&cap)) != NULL)
        strcpy(ext_keys.page_prev,ret);
    if((ret = (tgetstr)("kh",&cap)) != NULL)
        strcpy(ext_keys.home_key,ret);
    if((ret = (tgetstr)("kI",&cap)) != NULL)
        strcpy(ext_keys.insert_key,ret);
    if((ret = (tgetstr)("@7",&cap)) != NULL)
        strcpy(ext_keys.end_key,ret);
    if((ret = (tgetstr)("cl",&cap)) != NULL)
        strcpy(ext_keys.clear_screen,ret);

    return 0;
}

/*
 * termSetup :
 * saves the current settings of the terminal and invokes some signal
 * catching functions; it is a good idea to call this function as the first
 * thing in main(); if you do not call it as the first function in main
 * do not call it at all as it might save the wrong settings and put your
 * terminal into an undesirable state on exit
 * IN  : void
 * OUT : 0 on success IOERROR on failure
 */
int
termSetup(void)
{
	sigset_t set;
	struct sigaction act;


    if(first > 0) {
        if(extended_keys() == IOERROR)
            first = IOERROR;
        else
            first = 0;
    }
	if((tcgetattr)(STDIN_FILENO,&start_attr) < 0)
		return IOERROR;

	atexit(cleanup);

	if((sigemptyset)(&set) < 0)
		return IOERROR;
	if((sigaddset)(&set,SIGINT) < 0)
		return IOERROR;
	if((sigaddset)(&set,SIGQUIT) < 0)
		return IOERROR;
	if((sigaddset)(&set,SIGTSTP) < 0)
		return IOERROR;
	if((sigaddset)(&set,SIGTERM) < 0)
		return IOERROR;
	act.sa_mask = set;
	act.sa_flags = 0;
	act.sa_handler = resetMode;
	if((sigaction)(SIGCONT,&act,(struct sigaction*)NULL) < 0)
		return IOERROR;

	if((sigdelset)(&set,SIGTSTP) < 0)
		return IOERROR;
	act.sa_mask = set;
	act.sa_handler = saveMode;
	oact = act;
	if((sigaction)(SIGTSTP,&act,(struct sigaction*)NULL) < 0)
		return IOERROR;
	
	if((sigemptyset)(&set) < 0)
		return IOERROR;
	act.sa_mask = set;
	act.sa_handler=dieOnSignal;
	if((sigaction)(SIGINT,&act,(struct sigaction*)NULL) < 0)
		return IOERROR;

	if((sigaction)(SIGTERM,&act,(struct sigaction*)NULL) < 0)
		return IOERROR;

	return 0;
}

static int
timeout(void)
{
	struct timeval tv;
	fd_set readfds;
	int n;


    if(!(isatty)(STDIN_FILENO))
        return IOERROR;

	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO,&readfds);

	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = TIMEOUT_USEC + no_block;

	if((n = (select)(1,&readfds,(fd_set*)NULL,(fd_set*)NULL,&tv)) < 0)
		return IOERROR;

	return n;
}

void
cleanup(void)
{
    if((isatty)(STDIN_FILENO))
        (tcsetattr)(STDIN_FILENO,TCSAFLUSH,&start_attr);
}

void
saveMode(int sig)
{
	struct sigaction act;

	
    if((isatty)(STDIN_FILENO)) {
        (tcgetattr)(STDIN_FILENO,&save_attr);
        (tcsetattr)(STDIN_FILENO,TCSAFLUSH,&start_attr);
        (sigemptyset)(&act.sa_mask);
        act.sa_handler = SIG_DFL;
        act.sa_flags = 0;
        (sigaction)(sig,&act,(struct sigaction*)NULL);
        (kill)(getpid(),sig);
    }
}	

void
resetMode(int sig)
{
    if((isatty)(STDIN_FILENO)) {
        (tcsetattr)(STDIN_FILENO,TCSAFLUSH,&save_attr);
        (sigaction)(SIGTSTP,&oact,(struct sigaction*)NULL);
    }
}	

void
dieOnSignal(int sig)
{
    if((isatty)(STDIN_FILENO)) {
        (write)(1,ext_keys.clear_screen,strlen(ext_keys.clear_screen)); 
        (tcsetattr)(STDIN_FILENO,TCSAFLUSH,&start_attr);
        (kill)(getpid(),SIGKILL);
    }
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
***** DO NOT MIX THESE VERSIONS OF Raw() AND Cbreak() WITH THE *****
***** CURSES VERSIONS; EITHER USE THESE OR THE CURSES VERSIONS *****
all of the following functions return the constant IOERROR when an error
occurs or when stdin is not assciated with a terminal; make sure to check
for this return value these functions return 0 on correct functioning
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*
 * Raw :
 * put console into raw mode; no character processing by kernel
 * cbreak mode is preferable to raw mode
 * IN  : void
 * OUT : 0 on correct functioning; IOERROR on error
 */
int
Raw(void)
{
	struct termios attr;


    if(!(isatty)(STDIN_FILENO))
        return IOERROR;

	if(tty_status == g_RAW)
		return 0;

	if((tcgetattr)(STDIN_FILENO,&attr) < 0)
		return IOERROR;

	if(tty_status != g_CBREAK)
		orig_attr = attr;

	attr.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	attr.c_iflag &= ~(BRKINT | ICRNL | INLCR | INPCK | ISTRIP | IXON);
	attr.c_oflag &= ~OPOST;
	attr.c_cflag &= ~(CSIZE | PARENB);
	attr.c_cflag |= CS8;
	attr.c_cc[VMIN] = 1;
	attr.c_cc[VTIME] = 0;

	if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&attr) < 0)
		return IOERROR;

	tty_status = g_RAW;

	return 0;
}

/*
 * noRaw :
 * return console to cooked mode
 * IN  : void
 * OUT : 0 on correct functioning; IOERROR on error
 */
int
noRaw(void)
{
    if(!(isatty)(STDIN_FILENO))
        return IOERROR;

	if(tty_status != g_RAW && tty_status != g_CBREAK)
		return 0;

	if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&orig_attr) < 0)
		return IOERROR;

	tty_status = g_RESET;

	return 0;
}

/*
 * Cbreak :
 * put console into cbreak mode; partial character processing by kernel
 * only ^C (SIGINT), ^Z (SIGTSTP), and ^\ (SIGQUIT) are processed by kernel
 * IN  : void
 * OUT : 0 on correct functioning; IOERROR on error
 */
int
Cbreak(void)
{
	struct termios attr;


    if(!(isatty)(STDIN_FILENO))
        return IOERROR;

	if(tty_status == g_CBREAK || tty_status == g_RAW)
		return 0;

	if((tcgetattr)(STDIN_FILENO,&attr) < 0)
		return IOERROR;

	orig_attr = attr;

	attr.c_lflag &= ~(ICANON | ECHO);
	attr.c_cc[VMIN] = 1;
	attr.c_cc[VTIME] = 0;

	if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&attr) < 0)
		return IOERROR;

	tty_status = g_CBREAK;

	return 0;
}

/*
 * noCbreak :
 * return console to cooked mode
 * IN  : void
 * OUT : 0 on correct functioning; IOERROR on error
 */
int
noCbreak(void)
{
    if(!(isatty)(STDIN_FILENO))
        return IOERROR;

	if(tty_status != g_RAW && tty_status != g_CBREAK) 
		return 0;

	if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&orig_attr) < 0)
		return IOERROR;

	tty_status = g_RESET;

	return 0;
}

/*
 * noEcho :
 * turn off echo; characters typed will not be echoed to stdout
 * IN  : void
 * OUT : 0 on correct functioning; IOERROR on error
 */
int
noEcho(void)
{
	struct termios attr;


    if(!(isatty)(STDIN_FILENO))
        return IOERROR;

	if((tcgetattr)(STDIN_FILENO,&attr) < 0)
		return IOERROR;
	
	attr.c_lflag &= ~ECHO;

	if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&attr) < 0)
		return IOERROR;

	return 0;
}

/*
 * Echo :
 * put console into echo mode
 * IN  : void
 * OUT : 0 on correct functioning; IOERROR on error
 */
int
Echo(void)
{
	struct termios attr;


    if(!(isatty)(STDIN_FILENO))
        return IOERROR;

	if((tcgetattr)(STDIN_FILENO,&attr) < 0)
		return IOERROR;

	attr.c_lflag |= ECHO;

	if((tcsetattr)(STDIN_FILENO,TCSAFLUSH,&attr) < 0)
		return IOERROR;

	return 0;
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!  THE FOLLOWING FUNCTIONS ARE MEANT ONLY FOR CURSES THAT IS WHY THEY  !!!
!!!  BEGIN WITH THE LETTER "W"                                           !!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#ifdef USE_CURSES

/*
 * wgetkey :
 * this is only for use with curses!!! use getkey for other things
 * same as getkey except that the characters are echoed to the window if
 * they are the printable characters; this function only does blocking I/O
 * IN  : a pointer to the curses window
 * OUT : same as getkey
 */
int
wgetkey(WINDOW *win)
{
	int input;


	if((isprint)((input = getkey(BLOCKING,CBREAK_MODE,NULL))))
		waddch(win,input);

	return input;
}

/*
 * wgetline :
 * this is a curses routine, use it only for curses!!!
 * gets a line from the user echoing it to the window
 * make sure you position the cursor with wmove() to where you want to start
 * reading the characters from before calling wgetline() as that is how
 * wgetline determines where to start reading from
 * backspace will move cursor back and erase the character before the cursor
 * ^U will kill the entire line of input;
 * when the right side of the window is reached or characters typed would 
 * exceed the size of the buffer the console will beep
 * therefore the most characters that can be read on an 80 character
 * screen is 79; at most size-1 characters are read into buf
 * this function assumes that you have a border around the screen and will
 * not overwrite your border
 * IN  : window to read from
 *       buffer to read into
 *       size of buffer
 * OUT : number of characters read in
 */
int
wgetline(WINDOW *win,char *buf,int size)
{
	int input, i, row,col, startcol;


	getyx(win,row,col);

	startcol = col;

	for(i = 0;i < size - 1 && i < win->_maxx - startcol - 1;++i)
		waddch(win,'_');

	i = 0;
	wmove(win,row,col);
	wrefresh(win);

	while((input = getkey(BLOCKING,CBREAK_MODE,NULL)) != 0X0A 
			&& input != 0X0D) {
		if((input == 0X08 || input == 0X7F) && i > 0) {
			--i;
			--col;
			wmove(win,row,col);
			waddch(win,'_');
			wmove(win,row,col);
		} else if(input == 0X15 && i > 0) {
			col -= i;
			wmove(win,row,col);
			for(;i;--i)
				waddch(win,'_');
			wmove(win,row,col);
		} else if((isprint)(input) 
				&& i < (size - 1) && i < (win->_maxx - startcol - 1)) {
			++col;
			waddch(win,input);
			buf[i++] = input;
		} else if(i >= (size - 1) || i >= (win->_maxx - startcol - 1))
			(write)(1,"\a",1);
		wrefresh(win);
	}
	buf[i] = 0;

	return i;
}
/*
 * wgettoken :
 * this is a curses routine, use it only for curses
 * gets a line from the user echoing it to the window by calling wgteline()
 * the string is then searched for characters contained in toklim, if one is
 * found it is replaced by null to give you a single token;
 * toklim can contain any number of characters this function works very 
 * similar to strtok in string.h except that you only get the first token
 * IN  : window to read from
 *       buffer to read into
 *       size of buffer
 *       token delimiting characters 
 * OUT : number of characters in buffer
 */
int
wgettoken(WINDOW *win,char *buf,int size,char *toklim)
{
	int i, j, done;


	wgetline(win,buf,size);
	
	for(i = 0,done = 0;*buf;++buf,++i) {
		for(j = 0;*(toklim + j);++j) {
			if(*buf == *(toklim + j)) {
				done = 1;
				break;
			}
		}
		if(done)
			break;
	}
	*buf = 0;

	return i;
}

#endif /* USE_CURSES */
