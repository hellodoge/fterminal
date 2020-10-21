#ifndef FTERMINAL_PREFERENCES_H
#define FTERMINAL_PREFERENCES_H

#define CONFIG_FILE "fterminal.cfg"

#define DEFAULT_MASTER_PASSWORD "foobar"

#define UT_KEY_CANCEL KEY_F(2)

#define TERMINAL_NAME "HDG-V300"
#define TERMINAL_BOOT_AGENT_NAME "HDG Industries(TM) MF Boot Agent v2.2.0"
#define TERMINAL_BIOS_NAME "BOOBA BIOS"
#define TERMINAL_BIOS_VERSION "BBIOS-4.02.08.00 52EE5.E7.E8"
#define TERMINAL_COPYRIGHT "Copyright 2201-2203 HDG Ind."
#define TERMINAL_MEMORY "Uppermem: 64 KB"
#define TERMINAL_ROOT "Root (5A8)"

#define SHELL_MSG_WELCOME "WELCOME TO %s", SHELL_NAME
#define SHELL_NAME "ROBCO INDUSTRIES (TM) TERMLINK"

#define HACKING_MSG "ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL\n\
ENTER PASSWORD NOW"
#define HACKING_MSG_LINES 2

#define CMD_HELP "HELP"
#define CMD_EXIT "EXIT"
#define CMD_ECHO "ECHO"
#define CMD_ADD_USER "ADDUSR"
#define CMD_LIST_USERS "USRLIST"
#define CMD_DELETE_USER "DELUSR"
#define CMD_CHANGE_PASSWORD "NEWPASS"
#define CMD_LOGIN "LOGIN"
#define CMD_HACK "GETPTR"
#define CMD_CLEAR "CLEAR"

#define SHELL_PROMPT "> "
#define PASSWORD_SUB ('*' | A_BOLD)

#define MAX_ATTEMPTS 6
#define MIN_PASSWORD_LEN 4
#define MAX_PASSWORD_LEN 16
#define CURSOR A_STANDOUT

extern int currentY, currentX;
extern int maxY, maxX;

#endif //FTERMINAL_PREFERENCES_H
