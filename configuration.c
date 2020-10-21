#include "configuration.h"

#include <libconfig.h>
#include <sys/stat.h>

#include "preferences.h"

config_t config;
config_setting_t *setting, *root, *users;

const char* getMasterPassword() {
	root = config_root_setting(&config);
	const char *master;
	if (!config_setting_lookup_string(root, CFG_MASTER_PASSWORD, &master))
		return DEFAULT_MASTER_PASSWORD;
	return master;
}

const char* getUserPassword(char *username) {
	root = config_root_setting(&config);
	users = config_setting_get_member(root, CFG_USERS);
	if (!users)
		return NULL;
	const char *password;
	if (!config_setting_lookup_string(users, username, &password))
		return NULL;
	return password;
}

const char* getUsername(int n) {
	root = config_root_setting(&config);
	users = config_setting_get_member(root, CFG_USERS);
	if (!users)
		return NULL;
	setting = config_setting_get_elem(users, n);
	if (setting == NULL) return NULL;
	return config_setting_name(setting);
}

void setUserPassword(char *username, char *password) {
	root = config_root_setting(&config);
	users = config_setting_get_member(root, CFG_USERS);
	if (!users)
		users = config_setting_add(root, CFG_USERS, CONFIG_TYPE_GROUP);
	setting = config_setting_lookup(users, username);
	if (!setting)
		setting = config_setting_add(users, username, CONFIG_TYPE_STRING);
	config_setting_set_string(setting, password);
}

int deleteUser(char *username) {
	root = config_root_setting(&config);
	users = config_setting_get_member(root, CFG_USERS);
	if (!users)
		return 0;
	if (config_setting_remove(users, username))
		return 1;
	return 0;
}

int configurationInit() {
	config_init(&config);

	struct stat buffer;
	if (stat(CONFIG_FILE, &buffer)) {
		refillAttempts();
		if (writeConfig()) return 2;
	}

	if(! config_read_file(&config, CONFIG_FILE)) {
		fprintf(stderr, "%s - %s:%d", config_error_text(&config),
				config_error_file(&config), config_error_line(&config));
		config_destroy(&config);
		return 1;
	}

	if (getAttemptsCount() < 0) {
		refillAttempts();
		writeConfig();
	}

	return 0;
}

void configurationDestructor() {
	config_destroy(&config);
}

int writeConfig() {
	if (! config_write_file(&config, CONFIG_FILE)) {
		fprintf(stderr, "Error while writing config file.\n");
		config_destroy(&config);
		return 1;
	}
	return 0;
}

int getAttemptsCount() {
	int attempts;
	root = config_root_setting(&config);
	if (!config_setting_lookup_int(root, CFG_ATTEMPTS, &attempts))
		return -1;
	return attempts;
}

void reduceAttempts() {
	int attempts = getAttemptsCount();
	if (attempts <= 0) return;
	root = config_root_setting(&config);
	setting = config_setting_lookup(root, CFG_ATTEMPTS);
	config_setting_set_int(setting, --attempts);
}

void refillAttempts() {
	int attempts;
	root = config_root_setting(&config);
	if (!config_setting_lookup_int(root, CFG_ATTEMPTS, &attempts))
		setting = config_setting_add(root, CFG_ATTEMPTS, CONFIG_TYPE_INT);
	else
		setting = config_setting_lookup(root, CFG_ATTEMPTS);
	config_setting_set_int(setting, MAX_ATTEMPTS);
}