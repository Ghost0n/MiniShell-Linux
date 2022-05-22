#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//Implémentation de la commande cd
void cd(char **tokens)
{

	if (chdir(*(tokens+1)) != 0)
	{
		perror("Erreur de chdir: ");
	}
}


void execute_pipes(int t_index,char **tokens,char * cwd,int *pipe_indexs,int pip_count)
{
	char* argv[20];
	const char *cmd;
	int arg_count;
	int fd1[2];
	int fd2[2];
	pid_t pid;

		if (pipe(fd1) == -1) 
	{
			perror("pipe1");
			exit(EXIT_FAILURE);
	}
	if (pipe(fd2) == -1) 
	{
		perror("pipe2");
		exit(EXIT_FAILURE);
	}

	for (int i = pip_count;i>0;i--)
	{
		arg_count = 0;
		argv[0] = cwd;

		pid = fork();
		if (pid == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (pid == 0) {    /* Child reads from pipe */
			dup2(fd1[0], 0);
			dup2(fd2[1], 1);
			
			cmd = *(tokens+*(pipe_indexs+i-1)+1);
			for (int i = *(pipe_indexs+i-1)+2;i < *(pipe_indexs+i)-1;i++)
			{
				argv[i] = *(tokens+i);
				arg_count++;
			}
			argv[arg_count] = NULL;
			
			if (execvp(cmd, argv) == -1)
				perror(cmd);	
			exit(EXIT_SUCCESS);

		} else {            /* Parent writes argv[1] to pipe */
			dup2(fd1[1], 1);
			dup2(fd2[0], 0);
			if (execvp(cmd, argv) == -1)
				perror(cmd);

			wait(NULL);                /* Wait for child */
			exit(EXIT_SUCCESS);
		}
		
	}
}


//Cette fonction execute les commandes avec ses arguments entrée par l'utilisateur en utilisant fork(),waitpid et execve.
void command(int t_index,char **tokens,char * cwd,int in_index,int out_index,int stdr_out_index,int *pipe_indexs,int pip_count)
{
	//Initialisation des variables
	char* argv[20];
	pid_t pid = 0;
	int	status = 0;
	//Descripteur de Fichier de redirection de l'entrée/sortie
	int fd;
	//le premier mot de liste des tokens est la commande
	const char *cmd = *tokens; 

	
	//Constitution de la liste des arguments 
	argv[0] = cwd;
	
	for (int i = 1;i < t_index;i++)
	{
		argv[i] = *(tokens+i);
	}
	argv[t_index] = NULL;
	
	
	// On fork
	pid = fork();
	if (pid == -1)
		perror("Erreur de fork");
	// Si le fork n'a pas echoué le processus pere attend l'enfant (process fork)
	else if (pid > 0) {
		// On attend la fin d'execution de processus enfant puis on kill
		waitpid(pid, &status, 0);
		kill(pid, SIGTERM);
	} else {
		// Traiter ">" la redirection de sortie stdout
		if (pip_count>1)
		{
			execute_pipes(t_index,tokens,cwd,pipe_indexs,pip_count);
		}
		else if (out_index != 0)
		{
			fd = open(*(tokens+out_index), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			dup2(fd, 1);
			close(fd);
		}
		// Traiter "<" la redirection d'entrée
		else if (in_index != 0)
		{
			fd = open (*(tokens+in_index), O_RDONLY);
			dup2 (fd, 0);
			close (fd);
		}
		// Traiter "2>" la redirection de sortie stdrr
		else if (stdr_out_index != 0)
		{
			fd = open(*(tokens+stdr_out_index), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			dup2(fd, 2);
			close(fd);
		}
		
		// execve execute la commande avec ces arguments stocké dans argv
		if (execvp(cmd, argv) == -1)
			perror(cmd);
	}
}
