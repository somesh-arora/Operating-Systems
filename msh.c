/*
Somesh Arora
1001083238
Programming Assignment #1
Mav Shell
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line
#define MAX_COMMAND_SIZE 255    // The maximum command-line size
#define MAX_NUM_ARGUMENTS 10     // Mav shell only supports ten  arguments

int main()
{
	/*
	Declaring Dynamic array for storing 10 most recent pids
	Memset will initially set every value to 0
	*/
	int *pid= ((int*) malloc(10*(sizeof(int))));
	memset(pid, 0, 10*sizeof(int));
	char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

	while( 1 )
	{
		// Print out the msh prompt
		printf ("msh> ");

    		// Read the command from the commandline.  The
	 	// maximum command that will be read is MAX_COMMAND_SIZE
   		// This while command will wait here until the user
    		// inputs something since fgets returns NULL when there
    	  	// is no input
		
		/*
		While loop to determine the length for placing the value of PID
		at the right place
		*/
		int length=0,found=0;
		while(pid[length]!=0 && length<10)
		{
			length++;
		}
    		while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

		
		/*
		If User enters the Showpid command then based on the length value,
		Program will show the PID of processes executed recently,
		If the # of processes is more than 10, it will show the PID of 
		10 most recent processes. 
                */
		if(!strcmp(cmd_str,"showpid\n"))
		{
			int i;
			for(i=0;i<length;i++)
			{
				printf("%d\n",pid[i]);
			}
			continue;
		}
	    	/* Parse input */
    		char *token[MAX_NUM_ARGUMENTS];
    		int   token_count = 0;                                 
                                                  
    		// Pointer to point to the token
    		// parsed by strsep
   		char *arg_ptr;                                                                                    
    		char *working_str  = strdup( cmd_str );
                
		// we are going to move the working_str pointer so
		// keep track of its original value so we can deallocate
		// the correct amount at the end
		char *working_root = working_str;

		// Tokenize the input strings with whitespace used as the delimiter
		while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
		(token_count<MAX_NUM_ARGUMENTS))
    		{
      			token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      			if( strlen( token[token_count] ) == 0 )
      			{
        			token[token_count] = '\0';
      			}
        		token_count++;
    		}
		// If user does not enters any command, shell will quitely jumps to the
		// beginnning of the loop and waits for the user to enter the next command

		if(token[0]=='\0')
		{
			continue;
		}
		 /*
		If user types "exit" or "quit" then the shell with terminate with status 0
		*/

		if(  (strcmp(token[0],"exit"))==0 || (strcmp(token[0],"quit"))==0 )
		{
			exit(0);
		}

		// If user enter "cd", this command is not taken care by exec() so we have to take
		// Care of it manually.
		//If user does not type the path after "cd" then user will be prompted to mention the path
		// If user mentions an incorrect path then system will notify user to enter the correct path
		if(strcmp(token[0],"cd")==0)
		{
        		if(token[1]==NULL)
        		{
                		printf("Please mention path\n");
				continue;
        		}
        		else
        		{
                		if(chdir(token[1])!=0)
				{
					printf("Invalid path\n");
				}
				continue;

        		}
		}

		// 4 paths to search for the commands.
		char *str[]={"./","/usr/local/bin/","/usr/bin/","/bin/"};
		char var[100];
		int flag=-1;
		int i, pid_s;
		// Loop will run 4 times and check for the user entered command 
		// in the 4 given path. 
		for(i=0;i<4;i++)
		{
			// fork() the parent process
			pid_t child_pid = fork();
			pid_t tpid;
			int status;
			// string copy function will execute and str[i] will be copied to var
			// and strcat function will concatinate the var and token[0] to make the
			// correct user input for further execution
			strcpy(var,str[i]);
			strcat(var,token[0]);
			if( child_pid == 0 )
			{
				// execv() returns -1 if it fails, it does not return anything otherwise
				if(execv(var, token)==-1)
				{
				}
				exit(EXIT_FAILURE);
			}
			else
			{
				// If the command is not found in the given paths then
				// found value will be incremented and if found value reach
				// to the value of 4, command is not found in the given paths
				tpid= waitpid( child_pid, &status, 0 );
				if(WEXITSTATUS(status)==1)
				{
					found=found+1;
				}
				else
				{
					// If command is found in one of the four given paths
					// then value of pid will be stored and flag will be set 
					// to 1 to store value of PID later in the code.
					pid_s = child_pid;
					found=-1;
					flag=1;
				}
			}
		}
		// If user entered command is not found in the 4 given paths, system will
		// display a message of command not found.
		if(found==4)
		{
			printf("Command not found \n");
			found=0;
		}
		// Value of flag will turn to 1 when systen executes a valid process.
		if(flag==1)
		{
			// If the length is 10, the first PID should be truncated and
			// every other pid will get shifted by 1 place.
			// Most recent PID will be placed at last position. 
			if(length==10)
			{
				int j;
				for(j=0;j<=8;j++)
				{
					pid[j]=pid[j+1];
				}		
				length=length-1;
			}
			pid[length]=pid_s;
		}
  	  	free( working_root );
  	}
  	return 0;
}

