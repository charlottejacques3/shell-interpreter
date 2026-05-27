#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define HOST_MAX 25
#define MAX_ARG_LEN 100

int main() {
	// Determine prompt
	char cwd[PATH_MAX];
	char host[HOST_MAX];
	char* login = getlogin();
	gethostname(host, sizeof(host));
	char prompt[PATH_MAX+HOST_MAX+HOST_MAX] = "";
	int bailout = 0;

	while (!bailout) {
		getcwd(cwd, sizeof(cwd));
		snprintf(prompt, sizeof(prompt), "%s@%s: %s > ", login, host, cwd);
		char* reply = readline(prompt);

		// Ctrl-D (exit)
		if (reply == NULL) {
			bailout = 1;
			break;
		}

		// Tokenize response
		char *argv[MAX_ARG_LEN];
		int i = 0;
		argv[i] = strtok(reply, " ");
		while (argv[i] != NULL) {
			argv[++i] = strtok(NULL, " ");
		}

		// Change directory
		if (argv[0] != NULL && strcmp(argv[0], "cd") == 0) {
			char *home = getenv("HOME");
			if (argv[1] != NULL && strcmp(argv[1], "~") != 0) {
				char* newDir = argv[1];

				// Handle ~
				if (argv[1][0] == '~') {	
					newDir = home;
					argv[1]++;
					strcat(newDir, argv[1]);
				}
				if (chdir(newDir) != 0) {
					printf("cd: %s: No such file or directory\n", newDir);
				}
			} else {
				chdir(home);
			}
		}

		// Run foregroud processes
		else {
			pid_t pid = fork();
			if (pid < 0) {
				fprintf(stderr, "Error: fork failed\n");
				exit(-1);
			} else if (!pid) {
				
				// Run the process
				execvp(reply, argv);
				exit(0);
			}
			wait(NULL);
		}

		free(reply);
	}
	printf("Bye Bye\n");
	return 0;
}
