#include <stdlib.h>
#include <string.h>

#include "utilities.h"
#include "configuration.h"
#include "preferences.h"
#include "hacking.h"

void generateField();
void generateGuesses(const char* password, int passwordLen);
void updateAttempts();
int countCorrect(const char *password, char *guess, int length);

void hack(const char *password) {
	clear();
	getmaxyx(stdscr, maxY, maxX);
	printw(HACK_HEADER);

	int passwordLength = (int) strlen(password);

	generateField();
	generateGuesses(password, passwordLength);

	WINDOW *attemptWindow = newwin(maxY - HACK_HEADER_LINES - 4,
	                                maxX - (8 + CHARS_IN_ROW) * COL_COUNT,
	                               HACK_HEADER_LINES + 3, (8 + CHARS_IN_ROW) * COL_COUNT);
	scrollok(attemptWindow, true);

	while (getAttemptsCount() > 0) {
		updateAttempts();

		char guess[MAX_PASSWORD_LEN + 1];
		int attemptWindowMaxY = getmaxy(attemptWindow) - 1;
		mvwaddch(attemptWindow, attemptWindowMaxY, 0, '>');
		wmove(attemptWindow, attemptWindowMaxY, 2);
		wrefresh(attemptWindow);

		if (simpleInput(attemptWindow, guess, passwordLength, MODE_CAPITALIZE))
			break;
		int correct = countCorrect(password, guess, passwordLength);

		wscrl(attemptWindow, 4);
		if (correct == passwordLength) {
			mvwprintw(attemptWindow, attemptWindowMaxY - 3, 0,
			          "> Exact match!\n> Press any key...");
			wrefresh(attemptWindow);
			getch();
			break;
		} else {
			mvwprintw(attemptWindow, attemptWindowMaxY - 3, 0,
			          "> Entry denied\n> %d/%d correct.", correct, passwordLength);
			reduceAttempts();
		}

	}
	delwin(attemptWindow);
}

char getRandomTrashChar();

void generateField() {
	getmaxyx(stdscr, maxY, maxX);
	int rows = maxY - HACK_HEADER_LINES - 4;
	int addr = rand() % (0xFFFF - rows * COL_COUNT * CHARS_IN_ROW) / CHARS_IN_ROW * CHARS_IN_ROW;
	for (int col = 0; col < COL_COUNT; col++) {
		for (int row = 0; row < rows; row++) {
			mvprintw(row + HACK_HEADER_LINES + 3, col * (8 + CHARS_IN_ROW), "0x%04X ", addr);
			addr += CHARS_IN_ROW;
			for (int i = 0; i < CHARS_IN_ROW; i++)
				addch(getRandomTrashChar());
		}
	}
}

#define PUNCTUATION "!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~"
char getRandomTrashChar() {
	/* ASCII
	const int trashChars[4][2] = {
			{0x21, 0x2F - 0x21}, {0x3A, 0x40 - 0x3A}, {0x5B, 0x60 - 0x5B}, {0x7B, 0x7E - 0x7B}
	};
	int r = rand() % (trashChars[0][1] + trashChars[1][1] + trashChars[2][1] + trashChars[3][1]);
	int j;
	for (j = 0; r > trashChars[j][1]; j++) r -= trashChars[j][1];
	return trashChars[j][0] + r; */
	char *punctuation = PUNCTUATION;
	return punctuation[rand() % (sizeof(PUNCTUATION) - 1)];
}
#undef PUNCTUATION

#define CALCULATE_YX(y,x) \
	y = exactPosition % charsInCol / CHARS_IN_ROW + HACK_HEADER_LINES + 3; \
	x = 7 + positionInRow + (CHARS_IN_ROW + 8) * (exactPosition / charsInCol)
void placeGuess(int exactPosition, char *guess, int guessLen) {

	int positionInRow = exactPosition % CHARS_IN_ROW;
	getmaxyx(stdscr, maxY, maxX);
	const int charsInCol = CHARS_IN_ROW * (maxY - HACK_HEADER_LINES - 4);

	int placeY, placeX;
	CALCULATE_YX(placeY, placeX);
	do {
		mvprintw(placeY, placeX, "%.*s", CHARS_IN_ROW - positionInRow, guess);
		guessLen -= CHARS_IN_ROW - positionInRow;
		if (guessLen > 0) {
			guess += CHARS_IN_ROW - positionInRow;
			exactPosition += CHARS_IN_ROW - positionInRow;
			positionInRow = 0;
			CALCULATE_YX(placeY, placeX);
		}
	} while (guessLen > 0);
}

#undef CALCULATE_YX

void generateGuesses(const char* password, int passwordLen) {
	char *fileName = malloc(sizeof(GUESSES_FOLDER) + sizeof(char) * 2);
	sprintf(fileName, "%s/%d", GUESSES_FOLDER, passwordLen);
	FILE *fp = fopen(fileName, "r");
	free(fileName);

	long *guesses = malloc(sizeof(long));
	int lines = 1;
	*guesses = 0;
	for (int ch = 0; ch != EOF; ch = fgetc(fp)) { //TODO should do something with this bullshit code
		if (ch == '\n') {
			lines++;
			guesses = realloc(guesses, sizeof(long) * lines);
			*(guesses + lines - 1) = ftell(fp);
		}
	}

	getmaxyx(stdscr, maxY, maxX);
	int charsCount = (maxY - HACK_HEADER_LINES - 4) * CHARS_IN_ROW * COL_COUNT;
	int charsPerGuess = charsCount / GUESS_COUNT;

	int truePasswordIndex = rand() % GUESS_COUNT;
	char buffer[MAX_PASSWORD_LEN + 1];
	for (int i = 0; i < GUESS_COUNT; i++) {
		if (i == truePasswordIndex) {
			strncpy(buffer, password, passwordLen + 1);
		} else {
			int randomGuessIndex = rand() % lines;
			rewind(fp);
			fseek(fp, guesses[randomGuessIndex], SEEK_SET);
			fgets(buffer, passwordLen + 1, fp);
		}
		int guessPositionInScope = rand() % (charsPerGuess - passwordLen - 1);
		int guessPositionInField = charsPerGuess * i + guessPositionInScope;
		placeGuess(guessPositionInField, buffer, passwordLen);
	}

	free(guesses);
	fclose(fp);
}

void updateAttempts() {
	move(HACK_HEADER_LINES + 1, 0);
	printw("%d ATTEMPT(S) LEFT:", getAttemptsCount());
	for (int i = 0; i < MAX_ATTEMPTS; i++) {
		addch(' ');
		addch(' ' | (i < getAttemptsCount() ? A_STANDOUT : 0));
	}
	if (getAttemptsCount() == 1) {
		attron(A_BLINK);
		move(HACK_HEADER_LINES - 1, 0);
		printw("!!! WARNING: LOCKOUT IMMINENT !!!");
		attroff(A_BLINK);
	}
	refresh();
}

int countCorrect(const char *password, char *guess, int length) {
	int correct = 0;
	for (int i = 0; i < length; i++) {
		if (*guess == '\0') break;
		if (*password++ == *guess++)
			correct++;
	}
	return correct;
}
