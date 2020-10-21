#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "shell.h"
#include "configuration.h"
#include "preferences.h"
#include "utilities.h"

typedef struct command {
	char *name;
	int (*func) ();
	char *desc;
	char *helpMessage;
} command_t;


int cmdHelp();
int cmdEcho();
int cmdLogin();
int cmdAddUser();
int cmdDeleteUser();
int cmdNewPass();
int cmdListUsers();
int cmdClear();
int cmdHack();
int cmdExit();

int auth(char *username);
int newpass(char *username);
command_t *parseCommand(char *command);

const char *shellSharedBuffer;
const char *prompt = SHELL_PROMPT;

command_t commands[] = {
		{CMD_HELP, &cmdHelp, "[command]",
                "Display information about builtin commands."},
		{CMD_ECHO, &cmdEcho, "[argument]",
                "Write arguments to the standard output."},
		{CMD_LOGIN, &cmdLogin, "[username]",
				"Begin session on the system"},
		{CMD_ADD_USER, &cmdAddUser, "[username]",
                "Add a local user to the system"},
		{CMD_DELETE_USER, &cmdDeleteUser, "[username]",
				"Delete a user account and related files"},
		{CMD_CHANGE_PASSWORD, &cmdNewPass, "[username]",
				"Change user password"},
		{CMD_LIST_USERS, &cmdListUsers, 0,
                "Print information about users"},
		{CMD_HACK, &cmdHack, "[username]"},
		{CMD_CLEAR, &cmdClear, 0, "Clear the terminal screen"},
		{CMD_EXIT, &cmdExit, 0, "Exit the shell."}
};

int shell() {
	clear();
	scrollok(stdscr, true);
	printw(SHELL_MSG_WELCOME);
	char buffer[SHELL_BUFFER_SIZE];
	int result = 0;

	do {
		if (getAttemptsCount() <= 0)
			break;

		if (getcurx(stdscr))
			addch('\n');
		addstr(prompt);
		refresh();
		if (simpleInput(stdscr, buffer, SHELL_BUFFER_SIZE - 1,
				MODE_CAPITALIZE) == STATE_CANCELLED) {
			printw("\nCommand cancelled");
			continue;
		} else if (!strlen(buffer))
			continue;
		addch('\n');

		char *strCmd = strtok(buffer, " ");
		command_t *command = parseCommand(strCmd);

		if (!command) {
			printw("Command %s not found", strCmd);
			continue;
		}

		result = command->func();
	} while (!result);

	scrollok(stdscr, false);

	return result;
}

command_t *parseCommand(char *command) {
	for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
		if (!strcmp(command, commands[i].name)) {
			return &commands[i];
		}
	}
	return 0;
}

int cmdLogin() {
	char *username = strtok(NULL, " ");
	if (!username) {
		printw("%s command requires argument", CMD_LOGIN);
		return 0;
	}
	if (auth(username)) {
		shellSharedBuffer = username;
		return SHELL_RESULT_LOGIN;
	}
	return 0;
}

#define TMP_MSG_POINTER "READ --PTR 0x%04X > ACCOUNTS.F"
int cmdHack() {
	char *username = strtok(NULL, " ");
	if (!username) {
		printw("%s command requires argument", CMD_HACK);
		return 0;
	}
	shellSharedBuffer = getUserPassword(username);
	if (!shellSharedBuffer) {
		printw("User %s does not exist", username);
		return 0;
	}

	int fakePtr = rand() % 0xFFFF;
	printw("Local user's account found at 0x%04X\n", fakePtr);

	int s = fakeInput(stdscr, prompt, "SET TERMINAL/INQUIRE");
	if (s) goto cancel;
	printw("\n%s\n", TERMINAL_NAME);

	s = fakeInput(stdscr, prompt, "SET FILE/PROTECTION=OWNER:RWED ACCOUNTS.F");
	if (s) goto cancel;
	addch('\n');
	char *tmp = malloc(sizeof(TMP_MSG_POINTER));
	sprintf(tmp, TMP_MSG_POINTER, fakePtr);
	s = fakeInput(stdscr, prompt, tmp);
	free(tmp);
	if (s) goto cancel;
	addch('\n');

	s = fakeInput(stdscr, prompt, "SET HALT RESTART/MAINT");
	if (s) goto cancel;
	printw("\nInitializing %s\n%s\n%s\n%s\n%s\n%s\nMaintenance Mode\n",
			TERMINAL_BOOT_AGENT_NAME, TERMINAL_BIOS_NAME,
			TERMINAL_BIOS_VERSION, TERMINAL_COPYRIGHT,
			TERMINAL_MEMORY, TERMINAL_ROOT);

	s = fakeInput(stdscr, prompt, "RUN DEBUG/ACCOUNTS.F");
	if (s) goto cancel;
	return SHELL_RESULT_HACKING;

	cancel:
	printw("\nKernel crashed...\nPress any key to reboot");
	getch();
	return SHELL_RESULT_REBOOT;
}
#undef TMP_MSG_POINTER

int cmdExit() {
	printw("Press any key to shutdown...");
	getch();
	return SHELL_RESULT_EXIT;
}

int cmdHelp() {
	char *arg = strtok(NULL, " ");
	if (!arg) {
		printw("%s\nThese shell commands are defined internally.\nType %s to see this list.",
		       SHELL_NAME, CMD_HELP);
		for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
			printw("\n\t%s\t%s", commands[i].name, commands[i].desc ? commands[i].desc : "");
		}
	} else {
		command_t *command = parseCommand(arg);
		if (!command) {
			printw("Command %s not found", arg);
			return 0;
		}
		printw("Usage: %s %s\n", command->name, command->desc ? command->desc : "");
		if (command->helpMessage)
			addstr(command->helpMessage);
	}
	return 0;
}

int cmdEcho() {
	char *arg = strtok(NULL, "");
	if (arg)
		addstr(arg);
	else
		printw("%d attempts left", getAttemptsCount());
	return 0;
}

int cmdListUsers() { //TODO Correctly display long lists
	const char *username;
	addstr("Local users:\n");
	for (int i = 0; true; i++) {
		username = getUsername(i);
		if (!username) return 0;
		printw("%d. %s\n", i + 1, username);
	}
}

int cmdAddUser() {
	char *username = strtok(NULL, " ");
	if (!username) {
		printw("%s command requires argument", CMD_ADD_USER);
		return 0;
	}
	if (!isalpha(*username)) {
		addstr("Username must start with a letter");
		return 0;
	}
	for (char *cursor = username; *cursor != '\0'; cursor++) {
		if (!isalnum(*cursor)) {
			addstr("Username must be alphanumeric");
			return 0;
		}
	}
	const char *check = getUserPassword(username);
	if (check) {
		printw("User %s already exists", username);
		return 0;
	}
	newpass(username);
	return 0;
}

int cmdNewPass() {
	char *username = strtok(NULL, " ");
	if (!username) {
		printw("%s command requires argument", CMD_CHANGE_PASSWORD);
		return 0;
	}
	if (auth(username)) {
		newpass(username);
	}
	return 0;
}

int cmdDeleteUser() {
	char *username = strtok(NULL, " ");
	if (!username) {
		printw("%s command requires argument", CMD_DELETE_USER);
		return 0;
	}
	if (auth(username))
		if (!deleteUser(username))
			addstr("\nCannot delete user");
	return 0;
}

int cmdClear() {
	clear();
	return 0;
}

int auth(char *username) {
	const char *check = getUserPassword(username);
	if (!check) {
		printw("User %s does not exist", username);
		return 0;
	}
	char password[MAX_PASSWORD_LEN + 1];
	addstr("Enter password: ");
	if (simpleInput(stdscr, password, MAX_PASSWORD_LEN,
	                MODE_PASSWORD | MODE_CAPITALIZE) != STATE_CANCELLED) {
		if (!strcmp(password, check)) {
			return 1;
		} else {
			addstr("\nPassword does not match");
			reduceAttempts();
			printw("\n%d attempts left", getAttemptsCount());
		}
	} else {
		addstr("\nAuthentication was cancelled");
	}
	return 0;
}

int newpass(char *username) {
	char password[MAX_PASSWORD_LEN + 1];
	addstr("\nEnter new password: ");
	if (simpleInput(stdscr, password, MAX_PASSWORD_LEN,
	                MODE_PASSWORD | MODE_CAPITALIZE) != STATE_CANCELLED) {
		if (strlen(password) < MIN_PASSWORD_LEN) {
			printw("\nPassword is too short");
			return 0;
		}
		for (char *cursor = username; *cursor != '\0'; cursor++) {
			if (!isalpha(*cursor)) {
				addstr("Username must be alphabetic");
				return 0;
			}
		}
		setUserPassword(username, password);
		return 1;
	} else {
		addstr("\nPassword setting was cancelled");
	}
	return 0;
}