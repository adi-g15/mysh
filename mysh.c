#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

void builtin_cmd_stat (const char *input) {
	printf("Called builtin command 'stat', with input: %s\n", input);
}

void builtin_cmd_ls (const char *input) {
	printf("Called builtin command 'ls', with input: %s\n", input);
}

const char *registered_functions[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

void builtin_cmd_help (const char *input) {
	/* ignore input */
	printf("MySh Help:\n");
	printf("\tMySh is a basic shell, made to get some dopamine\n");
	printf("\n");
	printf("\tBuiltin commands:\n");
	printf("\t\tstat: time statistics of a file\n");
	printf("\t\tls: list files in current directory\n");
	printf("\t\thelp: show this help\n");
	printf("\t\tquit/q: exit the shell\n");
	printf("\n");
	printf("\tKeywords commands:\n");
	printf("\t\tfn: defines a function\n");
	printf("\n");
	printf("\tRegistered functions:\n");
	
	for (int i=0; i<(sizeof(registered_functions)/sizeof(registered_functions[0])); ++i)
		if (registered_functions[i] == NULL)
			break;
		else
		    printf("\t%d. %s\n", i+1, registered_functions[i]);

	printf("\n");
}

void builtin_cmd_quit (const char *input) {
	/* ignore input */
	exit(0);
}

const char *builtin_cmds_list[] = { "stat", "ls", "help", "q", "quit" };
enum builtin_cmds {
	CMD_STAT,
	CMD_LS,
	CMD_HELP,
	CMD_Q,
	CMD_QUIT
};

void (*builtin_cmds[])(const char *) = {
	builtin_cmd_stat,
	builtin_cmd_ls,
	builtin_cmd_help,
	builtin_cmd_quit,
	builtin_cmd_quit,
};

bool is_builtin_cmd (const char *cmd_name) {
	for (int i=0; i<(sizeof(builtin_cmds_list)/sizeof(builtin_cmds_list[0])); ++i) {
		if (strcmp(cmd_name, builtin_cmds_list[i]) == 0)
			return true;
	}

	return false;
}

enum keywords {
	KEYWORD_FN,
	KEYWORD_SYSTEM,
};

/*
 * Syntax for a function:
 *
 * fn my_name() {
 *
 * }
 *
 * */
void keyword_handler_fn (const char *input) {
	int open_paren_idx;
	char *fn_name = (char*)malloc(100);
	int fn_start_idx = 0, fn_end_idx = 0;
	char buffer[1024];
	int curr_buf_len = -1;
	int input_len = 0;

	if (strchr(input, '('))
		open_paren_idx = strchr(input, '(') - input;

	if (open_paren_idx == -1) {
		fprintf (stderr, "ERROR: Invalid syntax for function definition, please refer to the documentation\n");
		return;
	}

	input_len = strlen(input);

	for (fn_start_idx = 0; fn_start_idx < input_len; ++fn_start_idx)
		if (!isspace(input[fn_start_idx]))
			break;

	for (fn_end_idx = fn_start_idx; fn_end_idx < input_len; ++fn_end_idx)
		if (isspace(input[fn_end_idx]))
			break;

	/* Current `fn_end_idx` points to space or end of string, move it back one
	 * character */
	--fn_end_idx;

	memcpy(fn_name, input + fn_start_idx, fn_end_idx - fn_start_idx + 1);
	fn_name[fn_end_idx - fn_start_idx + 1] = '\0';

	while (1) {
		printf ("%s>", fn_name);
		fgets(buffer, sizeof(buffer), stdin);

		if (strchr(buffer, '}'))
			break;
	}

	for (int i=0; i<(sizeof(registered_functions)/sizeof(registered_functions[0])); ++i)
		if (registered_functions[i] == NULL) {
			registered_functions[i] = fn_name;
			break;
		}
}

void keyword_handler_system (const char *input) {
	system(input);
}

const char *keywords_list[] = { "fn", "system" };
void (*keyword_handlers[])(const char *) = {
	keyword_handler_fn,
	keyword_handler_system
};

bool is_keyword (const char *cmd_name) {
	for (int i=0; i<(sizeof(keywords_list)/sizeof(keywords_list[0])); ++i) {
		if (strcmp(cmd_name, keywords_list[i]) == 0)
			return true;
	}

	return false;
}

bool try_running_cmd (const char *cmd_name, const char* full_command) {
	char full_path[100];

	if (cmd_name[0] != '/') {
		memcpy(full_path, "/usr/bin/", sizeof("/usr/bin/"));

		memcpy(full_path + sizeof("/usr/bin/") - 1, cmd_name, strlen(cmd_name));
	} else {
		memcpy(full_path, cmd_name, strlen(cmd_name));
	}

	/* Checking `X_OK` to check if we have executable permission */
	int state = access(full_path, X_OK);
	if (access(full_path, X_OK) != 0) {
		return false;
	}

	system(full_command);

	return true;
}

int main(int argc, const char **argv) {
	char buf[128];
	int buf_len;
	char cmd_name[50];
	int cmd_start_idx, cmd_end_idx;
	int idx;

	while(1) {
		printf("MySh # ");
		fgets(buf, sizeof(buf), stdin);

		buf_len = strlen(buf);

		if (buf[buf_len - 1] == '\n') {
			--buf_len;
			buf[buf_len] = '\0';
		}

		for (cmd_start_idx = 0; cmd_start_idx < buf_len; ++cmd_start_idx)
			if (!isspace(buf[cmd_start_idx]))
				break;

		for (cmd_end_idx = cmd_start_idx+1; cmd_end_idx < buf_len; ++cmd_end_idx)
			if (isspace(buf[cmd_end_idx]))
				break;

		/* Currently `cmd_end_idx` points to a space, or at null character, move
		 * it back one character */
		--cmd_end_idx;

		memcpy(cmd_name, buf + cmd_start_idx, cmd_end_idx - cmd_start_idx + 1);
		cmd_name[cmd_end_idx - cmd_start_idx + 1] = '\0';

		if (is_builtin_cmd(cmd_name)) {
			for (idx = 0; idx < (sizeof(builtin_cmds_list)/sizeof(builtin_cmds_list[0])); ++idx)
				if (strcmp(cmd_name, builtin_cmds_list[idx]) == 0)
					break;

			/* Call the command's handler */
			builtin_cmds[idx](buf + cmd_end_idx + 1);
		} else if (is_keyword(cmd_name)) {
			for (idx = 0; idx < (sizeof(keywords_list)/sizeof(keywords_list[0])); ++idx)
				if (strcmp(cmd_name, keywords_list[idx]) == 0)
					break;

			/* Call the keyword handler */
			keyword_handlers[idx](buf + cmd_end_idx + 1);
		} else if (try_running_cmd(cmd_name, buf)) {
			/* Ran the command */
		} else {
			fprintf(stderr, "ERROR: Unknown command: %s\n", cmd_name);
		}
	}
}

