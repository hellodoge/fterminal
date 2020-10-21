#ifndef FTERMINAL_SHELL_H
#define FTERMINAL_SHELL_H

#define SHELL_BUFFER_SIZE 40

#define SHELL_RESULT_EXIT 1
#define SHELL_RESULT_HACKING 2
#define SHELL_RESULT_LOGIN 3
#define SHELL_RESULT_REBOOT 4

const char *shellSharedBuffer;

int shell();

#endif //FTERMINAL_SHELL_H
