#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "linked_list.h"

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

	Node* head = NULL;

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
		char *tokens[MAX_ARG_LEN]; //fix this 
		int i = 0;
		tokens[i] = strtok(reply, " ");
		while (tokens[i] != NULL) {
			tokens[++i] = strtok(NULL, " ");
		}

		if (tokens[0] != NULL) {

			// Change directory
			if (strcmp(tokens[0], "cd") == 0) {
				char *home = getenv("HOME");
				if (tokens[1] != NULL && strcmp(tokens[1], "~") != 0) {
					char* new_dir = tokens[1];

					// Handle ~
					if (tokens[1][0] == '~') {	
						new_dir = home;
						strcat(new_dir, tokens[1]+1);
					}
					if (chdir(new_dir) != 0) {
						fprintf(stderr, "cd: %s: No such file or directory\n", new_dir);
					}
				} else {
					chdir(home);
				}
			}

			// Run background process
			// else if (strcmp(argv[0], "bg") == 0) {
			// 	// same as foreground, except we don't wait?
			// } 

			// Run foregroud process
			else {
				// printf("Reply: %s\n", reply);

				int bg = 0;
				char **argv = tokens;
				char *proc = reply;
				// Check for background
				if (strcmp(proc, "bg") == 0) {
					bg = 1;
					if (tokens[1] != NULL) {
						proc = tokens[1];
						// argv = &argv[1];
						argv = &tokens[1];
						printf("BG stuff: %s, %s\n", argv[0], argv[1]);
					} else {
						fprintf(stderr, "Error: please provide a command to run in the background\n");
					}
				}
			
				pid_t pid = fork();
				if (pid < 0) {
					fprintf(stderr, "Error: fork failed\n");
					exit(-1);
				} else if (!pid) {
					printf("In child process\n");
					// Run the process
					head = add(head, pid, proc);
					printf("null? %d\n", head==NULL);
					print_lst(head);
					execvp(proc, argv);
					exit(0);
				} else if (!bg) {
					wait(NULL); //only wait if it's not a bg process
				}
			}
		}
		free(reply);
	}
	printf("Bye Bye\n");
	return 0;
}
