#ifndef FTERMINAL_UTILITIES_H
#define FTERMINAL_UTILITIES_H

#include <ncurses.h>


#define MODE_CAPITALIZE     0x1u
#define MODE_PASSWORD       0x1u << 1u

#define MODE_CURSOR_SET     0x0u
#define MODE_ERASE_PREVIOUS 0x1u
#define MODE_ABSOLUTE       0x1u << 1u | MODE_ERASE_PREVIOUS
#define MODE_RELATIVE       0x1u << 2u | MODE_ERASE_PREVIOUS
#define MODE_SET_NULL       0x1u << 3u
#define MODE_DISABLE        MODE_ERASE_PREVIOUS | MODE_SET_NULL

#define STATE_CANCELLED 1

int simpleInput(WINDOW *window, char *container, int len, unsigned mode);
int fakeInput(WINDOW *window, const char *prefix, const char *string);

int locked();

#endif //FTERMINAL_UTILITIES_H
