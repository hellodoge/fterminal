#ifndef FTERMINAL_CONFIGURATION_H
#define FTERMINAL_CONFIGURATION_H

#define CFG_ATTEMPTS "attempts"
#define CFG_USERS "users"
#define CFG_MASTER_PASSWORD "master"

int configurationInit();
void configurationDestructor();
int writeConfig();

const char* getUserPassword(char *username);
const char* getUsername(int n);
void setUserPassword(char *username, char *password);
int deleteUser(char *username);

int getAttemptsCount();
void refillAttempts();
void reduceAttempts();

const char* getMasterPassword();

#endif //FTERMINAL_CONFIGURATION_H
