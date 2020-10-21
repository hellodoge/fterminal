#include <ncurses.h>

#include <time.h>
#include <stdlib.h>

#include "hacking.h"
#include "utilities.h"
#include "configuration.h"
#include "shell.h"

int currentY, currentX;
int maxY, maxX;

int main() {

	if (configurationInit()) return 1;
	initscr();

	noecho();
	curs_set(0);
	keypad(stdscr, true);
	srand(time(NULL));

	int shellResult;
	do {
		if (getAttemptsCount() <= 0) {
			if (locked()) break;
			else refillAttempts();
		}

		shellResult = shell();
		switch (shellResult) {
			case SHELL_RESULT_LOGIN:
				clear();
				addstr("There's going to be something..."); //TODO
				getch();
				break;
			case SHELL_RESULT_HACKING:
				hack(shellSharedBuffer);
				break;
		}
		writeConfig();
	} while (shellResult != SHELL_RESULT_EXIT);

	endwin();
	configurationDestructor();

	return 0;
}
