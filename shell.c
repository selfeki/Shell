// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>



#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10
char history[HISTORY_DEPTH][COMMAND_LENGTH];
int histFull = 0;
int cmd_num = 1;




void shiftHistory(){
	for (int i=0; i<HISTORY_DEPTH-1;i++) {
		strcpy(history[i],history[i+1]);
	}
}
void histAddCommand(const char* cmd) {
	if (histFull == HISTORY_DEPTH) {
		shiftHistory();
		histFull--;
	}
	char temp[COMMAND_LENGTH];
	strcpy(temp,cmd);
	if (strrchr(cmd,'\n') == NULL) strcat(temp, "\n");
	strcpy(history[histFull],cmd);
	histFull++;
	return;
}


bool strtw(const char *str, const char *pref)
{
	if(strncmp(str, pref, strlen(pref)) == 0) {
		return 1;
	}
	return 0;
}

int getN (char* str){
	for (int i=1;i<strlen(str);i++) {
		if (!isdigit(str[i])) {
			write(STDOUT_FILENO,"failed !n\n", strlen("failed !n\n"));
			return 0;
		}
	}
	char tempstr[COMMAND_LENGTH];
	strcpy(tempstr, ++str);
	// write(STDOUT_FILENO, "\n", strlen("\n"));

	return atoi(str) ;
}



/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */
int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}



/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command_mod(char *buff, char *tokens[])
{

		// write(STDOUT_FILENO, "debug\n", strlen("debug\n"));

	// Read input

	if (buff== NULL) return;
	int length = strlen(buff);
	
	// write(STDOUT_FILENO, buff, strlen(buff));
		// write(STDOUT_FILENO, "debug\n", strlen("debug\n"));


	if ( (length < 0)  ){
		perror("Unable to read command. Terminating.\n");
		exit(-1);
	/* terminate with error */
	}
		// write(STDOUT_FILENO, "debug\n", strlen("debug\n"));

	// Null terminate and strip \n.
	buff[length] = '\0';

		// write(STDOUT_FILENO, "debug\n", strlen("debug\n"));

	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}
		// write(STDOUT_FILENO, "debug\n", strlen("debug\n"));

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
				// write(STDOUT_FILENO, "debug\n", strlen("debug\n"));

	}
}



void checkHistCommand(char* buff, char* tokes[]) {
	// write(STDOUT_FILENO, "!! detected \n", strlen("!! detected \n"));
	if (tokes[0] == NULL) return;
	// write(STDOUT_FILENO, "!! detected \n", strlen("!! detected \n"));
	if (!strcmp(tokes[0], "!!")) {
		// write(STDOUT_FILENO, "!! detected \n", strlen("!! detected \n"));

		//execute previous command
		if (cmd_num < 2) return ;
				// write(STDOUT_FILENO, "first\n", strlen("first\n"));
		if (histFull == HISTORY_DEPTH) {
			if (strlen(history[histFull-1]) > 0) {
				strcpy(buff, history[histFull-1]);
			}

		}
		else { 
			if (strlen(history[cmd_num-2]) > 0) {
				strcpy(buff, history[cmd_num-2]);
			}
		}
		read_command_mod(buff, tokes);
	}
	if (!(tokes[0][0] == '!')) return;
	int temp = getN(tokes[0]);
	if (temp > 0 && temp > cmd_num -9 && temp < cmd_num) {
		if (temp >= HISTORY_DEPTH) {
			strcpy(buff, history[(temp % (cmd_num -9))+1]);
		}
		else {
			strcpy(buff, history[(temp % 9)-1]);
			read_command_mod(buff, tokes);
		}
	}
	return ;
}
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);
	
	// write(STDOUT_FILENO, buff, strlen(buff));


	if ( (length < 0) ){
		perror("Unable to read command. Terminating.\n");
		exit(-1);
	/* terminate with error */
	}

	// Null terminate and strip \n.
	buff[length] = '\0';

	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}
	

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}
	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
	checkHistCommand(buff, tokens);


	histAddCommand(buff);
}


int checkInternalCommands(char* tokes[]) {

	if (tokes[0] == NULL) {
		return 1;
	}
	if (!strcmp (tokes[0],"exit")) {
		exit(1);	
	}
	if (!strcmp(tokes[0],"pwd")) {
		char wd[COMMAND_LENGTH];
		getcwd(wd, COMMAND_LENGTH);
		write(STDOUT_FILENO, wd , strlen(wd));
		write(STDOUT_FILENO, "\n", strlen("\n"));
		return 1;
	}
	if (!strcmp(tokes[0],"cd")) {
		int errval = chdir(tokes[1]);
		if (errval == -1) {
			write(STDOUT_FILENO, "Error in cd!\n"
				, strlen("Error in cd!\n"));
		}
		return 1;
	}
	if (!strcmp(tokes[0], "history")) {
		int idx = 0;
		if (cmd_num > HISTORY_DEPTH) {
			idx = cmd_num - 10;
		}
		for (int i=0;idx<cmd_num;i++) {
			idx++;
			char dumstr[4];
			sprintf( dumstr, "%d", idx);
			strcat(dumstr,"\t");
			write(STDOUT_FILENO, dumstr, strlen(dumstr));
			write(STDOUT_FILENO, history[i], strlen(history[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
		return 1;
	}
	return 0;
}

void handle_SIGINT() {
	write(STDOUT_FILENO, "\n", strlen("\n"));
	int idx = 0;
	if (cmd_num > HISTORY_DEPTH) {
		idx = cmd_num - 10;
	}
	for (int i=0;idx<cmd_num;i++) {
		idx++;
		char dumstr[4];
		sprintf( dumstr, "%d", idx);
		strcat(dumstr,"\t");
		write(STDOUT_FILENO, dumstr, strlen(dumstr));
		write(STDOUT_FILENO, history[i], strlen(history[i]));
		write(STDOUT_FILENO, "\n", strlen("\n"));
	}
		char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	char str[COMMAND_LENGTH];
	while (true) {
		char* prompt = strcat(getcwd(str,COMMAND_LENGTH),"> ");
		// Get commandf
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		write(STDOUT_FILENO, prompt, strlen(prompt));
		_Bool in_background = false;

		// if (checkHistCommand(&in_background, cmd_num) == 0) {
		// 	write(STDOUT_FILENO, "command not found\n", strlen("command not found\n"));
		// 	continue;
		// }


		read_command(input_buffer, tokens, &in_background);
		
		//////////

		// for (int i = 0; tokens[i] != NULL; i++) {
		// 	write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
		// 	write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
		// 	write(STDOUT_FILENO, "\n", strlen("\n"));
		// }


		/////
		int retval = checkInternalCommands(tokens);
		cmd_num++;
		if ( retval == 1) {
			continue;
		}
		int pid = fork();
		if (pid < 0) {
			write(STDOUT_FILENO,"fork() returned negative!\n", 
			strlen ("fork() returned negative!\n"));
		}
		if (pid == 0) {
			//child process
			int err = execvp(tokens[0], tokens);
			if (err == -1) {
					write(STDOUT_FILENO, "Command not found\n"
					, strlen("Command not found\n"));
			}
		return ;
		}
		if (pid > 0) {
			if (!in_background) {
				waitpid(pid, NULL,0);
			}
			// Cleanup any previously exited background child processes
			// (The zombies)
			while (waitpid(-1, NULL, WNOHANG) > 0)
				; // do nothing.	
		}
		// printf("ldkjflksdjflksdjflks");
		
		// //DEBUG: Dump out arguments:
		// for (int i = 0; tokens[i] != NULL; i++) {
		// 	write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
		// 	write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
		// 	write(STDOUT_FILENO, "\n", strlen("\n"));
		// }
		// if (in_background) {
		// 	write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
		// }

		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 */

		/*
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */
		//chdir(".");
	}
	return ;
}



/**hi
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	/* set up the signal handler */
	struct sigaction handler;
	handler.sa_handler = handle_SIGINT;
	handler.sa_flags = 0;
	sigaction(SIGINT, &handler, NULL);


	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	char str[COMMAND_LENGTH];
	while (true) {
		char* prompt = strcat(getcwd(str,COMMAND_LENGTH),"> ");
		// Get commandf
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		write(STDOUT_FILENO, prompt, strlen(prompt));
		_Bool in_background = false;

		// if (checkHistCommand(&in_background, cmd_num) == 0) {
		// 	write(STDOUT_FILENO, "command not found\n", strlen("command not found\n"));
		// 	continue;
		// }


		read_command(input_buffer, tokens, &in_background);
		
		//////////

		/////
		int retval = checkInternalCommands(tokens);
		cmd_num++;
		if ( retval == 1) {
			continue;
		}
		int pid = fork();
		if (pid < 0) {
			write(STDOUT_FILENO,"fork() returned negative!\n", 
			strlen ("fork() returned negative!\n"));
		}
		if (pid == 0) {
			//child process
			int err = execvp(tokens[0], tokens);
			if (err == -1) {
					write(STDOUT_FILENO, "Command not found\n"
					, strlen("Command not found\n"));
			}
		return 0;
		}
		if (pid > 0) {
			if (!in_background) {
				waitpid(pid, NULL,0);
			}
			// Cleanup any previously exited background child processes
			// (The zombies)
			while (waitpid(-1, NULL, WNOHANG) > 0)
				; // do nothing.	
		}
		// printf("ldkjflksdjflksdjflks");
		
		//DEBUG: Dump out arguments:
			// for (int i = 0; tokens[i] != NULL; i++) {
			// 	write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			// 	write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			// 	write(STDOUT_FILENO, "\n", strlen("\n"));
			// }
			// if (in_background) {
			// 	write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
			// }

		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 */

		/*
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */
		//chdir(".");
	}
	return 0;
};


//ERRORS ARE BECAUSE STRCAT APPENDS SRC TO DEST NOT JUST CONCATENATES!!

// IDEA FOR !n : 

// !n = history [n%(num-9)];