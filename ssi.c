#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "linked_list.h"

#define HOST_MAX 25
#define MAX_ARG_LEN 100

Node* check_bg_processes(Node* head);
char** tokenize(char* input, int* count);
void cd(char* dir);
Node* run_process(char* proc, char** argv, int bg, Node* head, char* full_name);
void signal_handler(int signum);

volatile sig_atomic_t interrupt = 0;

int main() {
	signal(SIGINT, signal_handler);
	// Determine prompt
	char cwd[PATH_MAX];
	char host[HOST_MAX];
	char* login = getlogin();
	gethostname(host, sizeof(host));
	char prompt[PATH_MAX+HOST_MAX+HOST_MAX] = "";
	int bailout = 0;

	Node* head = NULL;

	while (!bailout) {

		// Check for finished background processes
		head = check_bg_processes(head);

		getcwd(cwd, sizeof(cwd));
		snprintf(prompt, sizeof(prompt), "%s@%s: %s > ", login, host, cwd);
		char* reply = readline(prompt);

		// Ctrl-D (exit)
		if (reply == NULL) {
			bailout = 1;
			break;
		} else if (interrupt) {
			interrupt = 0;
			free(reply);
			continue;
		}

		// Tokenize response
		int token_count = 0;
		char **tokens = tokenize(reply, &token_count);

		if (tokens[0] != NULL) {

			// Change directory
			if (strcmp(tokens[0], "cd") == 0) {
				cd(tokens[1]);
			}

			// List background processes
			else if (strcmp(tokens[0], "bglist") == 0) {
				print_lst(head);
			}

			// Run processes
			else {
				// Check for background
				if (strcmp(tokens[0], "bg") == 0) {
					if (tokens[1] != NULL) {
						head = run_process(tokens[1], &tokens[1], 1, head, reply);
					} else {
						fprintf(stderr, "Error: please provide a command to run in the background\n");
					}
				} else {
					run_process(tokens[0], tokens, 0, NULL, reply);
				}
			}
		}

		// Free pointers
		free(reply);
		for (int i = 0; i < token_count; i++) {
			free(tokens[i]);
		}
		free(tokens);
	}
	printf("Bye Bye\n");
	return 0;
}


Node* check_bg_processes(Node* head) {
	Node* cur = head;
	int stat;
	while (cur != NULL) {
		if (waitpid(cur->pid, &stat, WNOHANG) > 0) {
			printf("%d: %s has terminated.\n", cur->pid, cur->name);
			Node* toRemove = cur;
			cur = cur->next;
			head = remove_by_pid(head, toRemove->pid);
		} else {
			cur = cur->next;
		}
	}
	return head;
}


char** tokenize(char* input, int *count) {

	char* str_cpy = strdup(input);

	// Allocate space for array
	int initial_capacity = 5;
	char** tokens = (char**)malloc(initial_capacity*sizeof(char*));
	if (tokens == NULL) {
		fprintf(stderr, "Error: malloc failed in tokenize()\n");
	}

	// Split input into tokens
	int i = 0;
	char* token = strtok(str_cpy, " ");
    while (token != NULL) {
        if (i >= initial_capacity - 1) {
            initial_capacity *= 2;
            tokens = realloc(tokens, initial_capacity*sizeof(char*));
        }
        tokens[i++] = strdup(token);
        token = strtok(NULL, " ");
    }
	tokens[i] = NULL;
	*count = i;
	return tokens;
}


void cd(char* dir) {
	char *home = getenv("HOME");
	if (dir != NULL && strcmp(dir, "~") != 0) {
		char path[PATH_MAX];
		strcpy(path, dir);
		if (dir[0] == '~') { // Handle ~
			snprintf(path, sizeof(path), "%s%s", home, dir + 1);
		}
		if (chdir(path) != 0) {
			fprintf(stderr, "cd: %s: No such file or directory\n", path);
		}
	} else { // Empty input
		chdir(home); 
	}
}


Node* run_process(char* proc, char** argv, int bg, Node* head, char* full_name) {
	pid_t pid = fork();
	if (pid < 0) {
		perror("Fork failed");
		exit(EXIT_FAILURE);
	} else if (!pid) {
		signal(SIGINT, SIG_DFL);
		execvp(proc, argv);
		perror(proc); // Should only get here if there's a failure
		exit(EXIT_FAILURE);
	} else if (bg) { // Background process
		head = add(head, pid, full_name);
		return head;
	} else {
		waitpid(pid, NULL, 0); // Only wait if it's not a background process
		return NULL;
	}
}

void signal_handler(int signum) {
	interrupt = 1;
}