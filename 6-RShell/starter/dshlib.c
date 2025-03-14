#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

int free_cmd_buff(cmd_buff_t *cmd_buff) {
  free(cmd_buff->_cmd_buffer);
  cmd_buff->argc = 0;
  return OK;
}

int free_cmd_list(command_list_t *clist) {
  for (int i = 0; i < clist->num; ++i) {
    free_cmd_buff(&clist->commands[i]);
  }
  clist->num = 0;
  return OK;
}

int execute_pipeline(command_list_t *clist) {
  int pipes[clist->num - 1][2];
  pid_t pids[clist->num];

  for (int i = 0; i < clist->num - 1; ++i) {
    if (pipe(pipes[i]) == -1) return ERR_EXEC_CMD;
  }

  for (int i = 0; i < clist->num; ++i) {
    pids[i] = fork();

    if (pids[i] == -1) return ERR_EXEC_CMD;
    if (pids[i] == 0) {
      if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
      if (i < clist->num - 1) dup2(pipes[i][1], STDOUT_FILENO);
      for (int j = 0; j < clist->num - 1; ++j) {
        close(pipes[j][0]);
        close(pipes[j][1]);
      }
      cmd_buff_t *cmd = &clist->commands[i];
      execvp(cmd->argv[0], cmd->argv);
    }
  }

  for (int i = 0; i < clist->num - 1; ++i) {
    close(pipes[i][0]);
    close(pipes[i][1]);
  }

  for (int i = 0; i < clist->num; ++i) {
    waitpid(pids[i], NULL, 0);
  }
  return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
  clist->num = 0;

  char *token;
  char *rest = cmd_line;
  int err;

  token = strtok_r(cmd_line, PIPE_STRING, &rest);

  while (token != NULL) {
    if (clist->num >= CMD_MAX) return ERR_TOO_MANY_COMMANDS;
    if ((err = build_cmd_buff(token, &clist->commands[clist->num])) != OK) return err;
    clist->num++;
    token = strtok_r(NULL, PIPE_STRING, &rest);
  }

  if (clist->num == 0) return WARN_NO_CMDS;
  return OK;
}

int parse_quoted_arg(char **cur, cmd_buff_t *cmd) {
  if (cmd->argc + 1 >= CMD_ARGV_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;

  char *arg = *cur + 1;
  int len = strcspn(arg, "\"\0");
  if (len > ARG_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
  if (arg[len] == '\0') return ERR_CMD_ARGS_BAD;
  arg[len] = '\0';
  *cur = arg + len;
  cmd->argv[cmd->argc++] = arg;
  return OK;
}

int parse_arg(char **cur, cmd_buff_t *cmd) {
  if (cmd->argc + 1 >= CMD_ARGV_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;

  char *arg = *cur;
  int len = strcspn(arg, " \0");
  if (len > ARG_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
  if (arg[len] == '\0') {
    *cur = arg + len - 1;
  } else {
    arg[len] = '\0';
    *cur = arg + len;
  }
  cmd->argv[cmd->argc++] = arg;
  return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd) {
  cmd->argc = 0;
  cmd->_cmd_buffer = malloc(sizeof(char) * (strlen(cmd_line) + 1));
  strcpy(cmd->_cmd_buffer, cmd_line);
  char *cur = cmd->_cmd_buffer;
  int err = OK;

  while (*cur != '\0') {
    switch (*cur) {
      case ' ':
        break;
      case '"':
        err = parse_quoted_arg(&cur, cmd);
        break;
      default:
        err = parse_arg(&cur, cmd);
    }
    if (err != OK) return err;
    ++cur;
  }

  cmd->argv[cmd->argc] = NULL;
  if (cmd->argc == 0) return WARN_NO_CMDS;
  if (strlen(cmd->argv[0]) > EXE_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
  return OK;
}

Built_In_Cmds match_command(const char *input) {
  if (strcmp(input, "exit") == 0) return BI_CMD_EXIT;
  if (strcmp(input, "cd") == 0) return BI_CMD_CD;
  return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
  switch (match_command(cmd->argv[0])) {
  case BI_CMD_EXIT:
    return BI_CMD_EXIT;
  case BI_CMD_CD:
    if (cmd->argc > 1) chdir(cmd->argv[1]);
    return BI_EXECUTED;
  default:
    return BI_NOT_BI;
  }
}

int exec_cmd(cmd_buff_t *cmd) {
  pid_t pid;
  int status;

  pid = fork();
  if (pid < 0) return ERR_EXEC_CMD;
  if (pid == 0) {
    if (execvp(cmd->argv[0], cmd->argv) < 0) return ERR_EXEC_CMD;
  }

  if (waitpid(pid, &status, 0) < 0) return ERR_EXEC_CMD;

  return WEXITSTATUS(status);
}


/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char *cmd_line;
    int rc = 0;
    command_list_t clist;

    cmd_line = malloc(sizeof(char) * SH_CMD_MAX);
    if (cmd_line == NULL) return ERR_MEMORY;

    while (1) {
      printf("%s", SH_PROMPT);

      if (fgets(cmd_line, ARG_MAX, stdin) == NULL) {
        printf("\n");
        break;
      }

      cmd_line[strcspn(cmd_line, "\n")] = '\0';

      rc = build_cmd_list(cmd_line, &clist);

      if (rc == WARN_NO_CMDS) {
        printf(CMD_WARN_NO_CMD);
      }
      if (rc == ERR_TOO_MANY_COMMANDS) {
        printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
      }
      if (rc != OK) continue;

      if (clist.num > 1) {
        rc = execute_pipeline(&clist);
      } else {
        cmd_buff_t *cmd = clist.commands;
        Built_In_Cmds bi_status = exec_built_in_cmd(cmd);

        if (bi_status == BI_NOT_BI) rc = exec_cmd(cmd);
        else if (bi_status == BI_CMD_EXIT) break;
        else rc = OK;
      }
      
      free_cmd_list(&clist);
    }

    free_cmd_list(&clist);
    free(cmd_line);
    return rc;
}
