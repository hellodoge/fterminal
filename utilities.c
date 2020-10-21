#include "utilities.h"

#include <ncurses.h>
#include <ctype.h>

#include "configuration.h"
#include "preferences.h"

int cursorY = -1, cursorX = -1;
WINDOW *cursorWindow = NULL;
chtype cursorChar = 0;

void setCursor(WINDOW *win, int y, int x, unsigned mode) {
	getyx(win, currentY, currentX);

	if (mode & MODE_ERASE_PREVIOUS)
		mvwaddch(cursorWindow, cursorY, cursorX, cursorChar);
	if (mode & MODE_SET_NULL) {
		cursorY = cursorX = -1;
		cursorWindow = NULL;
		cursorChar = 0;
		return;
	}
	if (mode & MODE_RELATIVE) {
		y = cursorY + y;
		x = cursorX + x;
	}

	cursorY = y;
	cursorX = x;
	cursorWindow = win;
	cursorChar = mvwinch(win, y, x);

	mvwaddch(cursorWindow, y, x,
	         cursorChar & CURSOR ? cursorChar & ~CURSOR : cursorChar | CURSOR);
	wmove(win, currentY, currentX);
	wrefresh(win);
}

int simpleInput(WINDOW *window, char *container, int len, unsigned mode) {
	char *cursor = container;
	int state = 0;
	int currentChar;
	getyx(window, currentY, currentX);
	setCursor(window, currentY + 1, currentX + 1, MODE_CURSOR_SET);
	do {
		currentChar = getch();
		if (isprint(currentChar)) {
			if (cursor - container >= len) continue;
			setCursor(window, 0, 1, MODE_RELATIVE);
			if (mode & MODE_CAPITALIZE)
				currentChar = toupper(currentChar);
			waddch(window, mode & MODE_PASSWORD ? PASSWORD_SUB : currentChar);
			*cursor++ = (char) currentChar;
		} else if (currentChar == KEY_BACKSPACE) {
			getyx(window, currentY, currentX);
			if (cursor > container) {
				cursor--;
				mvwaddch(window, currentY, currentX - 1, ' ');
				setCursor(window, 0, -1, MODE_RELATIVE);
				wmove(window, currentY, currentX - 1);
			}
		} else if (currentChar == UT_KEY_CANCEL) {
			state = STATE_CANCELLED;
			break;
		}
		wrefresh(window);
	} while (currentChar != '\n');
	*cursor = '\0';
	setCursor(window, 0, 0, MODE_DISABLE);
	return state;
}

int fakeInput(WINDOW *window, const char *prefix, const char *string) {
	addstr(prefix);
	const char *cursor = string;
	int currentKey, state = 0;
	getyx(window, currentY, currentX);
	setCursor(window, currentY + 1, currentX + 1, MODE_CURSOR_SET);
	do {
		currentKey = getch();
		if (isprint(currentKey) && *cursor != '\0') {
			setCursor(window, 0, 1, MODE_RELATIVE);
			waddch(window, *cursor++);
		} else if (currentKey == KEY_BACKSPACE && cursor > string) {
			cursor--;
			getyx(window, currentY, currentX);
			mvwaddch(window, currentY, currentX - 1, ' ');
			setCursor(window, 0, -1, MODE_RELATIVE);
			wmove(window, currentY, currentX - 1);
		} else if (currentKey == UT_KEY_CANCEL) {
			state = STATE_CANCELLED;
			break;
		}
		wrefresh(window);
	} while (*cursor != '\0' || currentKey != '\n');
	setCursor(window, 0, 0, MODE_DISABLE);
	return state;
}

#define TMP_MSG_LOCKED "TERMINAL LOCKED"
#define TMP_MSG_ORDER "PLEASE CONTACT AN ADMINISTRATOR"
int locked() {
	clear();
	getmaxyx(stdscr, maxY, maxX);
	mvprintw(maxY/2 - 1, maxX/2 - (int) sizeof(TMP_MSG_LOCKED) / 2, TMP_MSG_LOCKED);
	mvprintw(maxY/2 + 1, maxX/2 - (int) sizeof(TMP_MSG_ORDER) / 2, TMP_MSG_ORDER);

	int currentKey;
	const char *masterPass = getMasterPassword();
	const char *cursor = masterPass;

	do {
		currentKey = getch();
		if (toupper(currentKey) == toupper(*cursor))
			cursor++;
		else
			cursor = masterPass;
		if (*cursor == '\0') return 0;
	} while (currentKey != UT_KEY_CANCEL);
	return 1;
}
#undef TMP_MSG_LOCKED
#undef TMP_MSG_ORDER