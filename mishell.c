#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "commandes.h"
#define max_size 512

//Index qui parcourt le tableau de tokens
int t_index;

/*Cette fonction met les mots de la chaîne de caractères séparés par des espaces dans la liste des chaînes "tokens" 
et change la valeur de t_index pour indiquer la dernière case de la liste
Elle fait ça sur deux passes, la premiere pour parser les mots, la deuxième pour se débarasser des mots vides
qui peut être présent si l'utilisateur tape plusieurs espaces*/
void parser(char **tokens, char *Input)
{
	int t_index0 = 0;
	char **tokens0;
	tokens0 = (char **)malloc( sizeof(void *)*max_size/2 );
	
	while ( (*(tokens0+t_index0) = strsep(&Input," ")) != NULL)
	{
		t_index0++;
	}
	t_index = 0;
	
	for (int i = 0;i < t_index0;i++)
	{
		if (**(tokens0+i) != '\0')
		{
			*(tokens+t_index) = *(tokens0+i);
			t_index++;
		}
	}
}

//Compter le nombre des pipes présentes dans la commande (0 s'il n'y a pas de pipe intuitivement)
int count_pipes(char **tokens,int *pipe_indexs){
	
	int pip_count=1;
	for (int i=1;i<t_index;i++)
	{
		if (strcmp(*(tokens+i), "|") == 0)
		{
			*(pipe_indexs+pip_count) = i;
			pip_count++;
		}
		//S'il y a minimum une pipe on considère qu'il y a une autre au position -1 pour faciliter leurs traitement après
		*pip_count = -1;
		*(pip_count+pip_count) = t_index;
	}
	return pip_count;
}

//Cette fonction reconnait les commandes et appelle la fonction qui l'est associé 
void execute_commands(char **tokens,char *cwd,int *pipe_indexs)
{
	int not_redirected = 1;
	int pip_count = count_pipes(tokens,pipe_indexs);
	if (strcmp(*tokens, "exit") == 0)
	{
		//Fin de programme avec code 0: succès
		exit(0);
		
	}else if (strcmp(*tokens, "cd") == 0)
	{
		//on execute la commande cd
		cd(tokens);
	}
	else if (pip_count > 1)
	{
		command(i,tokens,cwd,i+1,0,0,pipe_indexs,pip_count);
	}
	else
	{
		//Test s'il y a une redirection de l'entrée/sortie et appelle la commande avec les arguments convenables			
		for (int i = 1;i < t_index;i++)
		{
			if ((strcmp(*(tokens+i), ">") == 0))
			{
				command(i,tokens,cwd,0,i+1,0,pipe_indexs,pip_count);
				not_redirected = 0;
				break;
			}
			else if ((strcmp(*(tokens+i), "2>")) == 0)
			{
				command(i,tokens,cwd,0,0,i+1,pipe_indexs,pip_count);
				not_redirected = 0;
				break;
			}
			else if (strcmp(*(tokens+i), "<") == 0)
			{
				command(i,tokens,cwd,i+1,0,0,pipe_indexs,pip_count);
				not_redirected = 0;
				break;
			}
		}
		//s'il n'y a pas de redirection on appelle la commande avec les arguments standards
		if (not_redirected)
			command(t_index,tokens,cwd,0,0,0,pipe_indexs,pip_count);
	}
}


int main()
{
//On stocke l'entrée de l'utilisateur dans la variable Input
	char *Input;
//Liste de chaines pour stocker les tokens parsés
	char **tokens;
//chaîne qui stocke le répertoire courant	
	char *cwd;
//Tableau d'entier qui stockera les indexes des pipe
	int *pipe_indexs;
	
//Allocation des variables
	Input = (char *)malloc( sizeof(char)*max_size );
	tokens = (char **)malloc( sizeof(void *)*max_size/2 );
	cwd = (char *)malloc( sizeof(char)*max_size );
	pipe_indexs = (int *)malloc( sizeof(int)*max_size/2 );
//Boucle principale du MiniShell   
	while(1)
		{
//Récupération et affichage du répertoire courant avant "%"
			getcwd(cwd, max_size);
			printf("%s %% ",cwd);
			
//fgets retourne NULL si le caractère eof est entré soit ctrl+D
			if (fgets(Input,max_size,stdin) == NULL) {
				
				printf("\n");
//exit(0) termine le programme normalement (avec succès)
				exit(0);
				}
			else
			{
//On remplace le dernier caractère qui est "\n" par le caractère de fin de chaîne "\0"
				*(Input + strlen(Input) - 1) = '\0';
//Parser l'entrée en tokens 
				parser(tokens,Input);
//Execute les commande 	
				execute_commands(tokens,cwd,pipe_indexs);
			}
		}
    return 0;
}
