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
	getcwd(cwd, sizeof(cwd));
	char* login = getlogin();
	gethostname(host, sizeof(host));
	char prompt[PATH_MAX+HOST_MAX+HOST_MAX] = "";
	snprintf(prompt, sizeof(prompt), "%s@%s: %s > ", login, host, cwd);
	int bailout = 0;

	while (!bailout) {
		char* reply = readline(prompt);
		/* Note that readline strips away the final \n */

		if (reply == NULL) {
			bailout = 1;
			break;
		}

		pid_t pid = fork();
		if (pid < 0) {
			fprintf(stderr, "Error: fork failed\n");
			exit(-1);
		} else if (!pid) {
			// Tokenize response
			char *argv[MAX_ARG_LEN];
			int i = 0;
			argv[i] = strtok(reply, " ");
			while(argv[i] != NULL) {
				argv[++i] = strtok(NULL, " ");
			}
			// Run the process
			execvp(reply, argv);
			exit(0);
		}
		wait(NULL);

		free(reply);
	}
	printf("Bye Bye\n");
	return 0;
}
