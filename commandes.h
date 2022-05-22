#ifndef COMMANDES_H
#define COMMANDES_H

void cd(char **tokens);
void command(int t_index,char **tokens,char * cwd,int in_index,int out_index,int stdr_out_index,int *pipe_indexs,int pip_count);
void execute_pipes(int t_index,char **tokens,char * cwd,int *pipe_indexs,int pip_count);

#endif
