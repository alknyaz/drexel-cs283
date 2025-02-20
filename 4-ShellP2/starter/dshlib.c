#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

int init_cmd_buff(cmd_buff_t *cmd_buff) {
  cmd_buff->_cmd_buffer = malloc(sizeof(char) * SH_CMD_MAX);
  if (cmd_buff->_cmd_buffer == NULL) return ERR_MEMORY;
  return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
  free(cmd_buff->_cmd_buffer);
  return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd) {
  cmd->argc = 0;
  strcpy(cmd->_cmd_buffer, cmd_line);
  char *cur = cmd->_cmd_buffer;

  while (cmd->argc + 1 < CMD_ARGV_MAX) {
    while (*cur == ' ') ++cur;
    if (*cur == '\0') break;

    char *arg = cur;

    if (arg[0] == '"') {
      ++arg;
      int len = strcspn(arg, "\"\0");
      arg[len] = '\0';
      cur += len + 2;
    }

    cmd->argv[cmd->argc++] = arg;

    cur += strcspn(cur, " \0");
    if (*cur == '\0') break;
    *cur = '\0';
    ++cur;
  }

  cmd->argv[cmd->argc] = NULL;

  if (cmd->argc == 0) return WARN_NO_CMDS;
  return OK;
}

Built_In_Cmds match_command(const char *input) {
  if (strcmp(input, "exit") == 0) return BI_CMD_EXIT;
  if (strcmp(input, "cd") == 0) return BI_CMD_CD;
  return BI_NOT_BI;
}

Built_In_Cmds exec_bi_cmd_cd(cmd_buff_t *cmd) {
  if (cmd->argc > 1) chdir(cmd->argv[1]);
  return BI_EXECUTED;
}

Built_In_Cmds exec_bi_cmd_exit(cmd_buff_t *cmd) {
  clear_cmd_buff(cmd);
  exit(0);
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
  switch (match_command(cmd->argv[0])) {
  case BI_CMD_EXIT:
    return exec_bi_cmd_exit(cmd);
  case BI_CMD_CD:
    return exec_bi_cmd_cd(cmd);
  default:
    return BI_EXECUTED;
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
    cmd_buff_t cmd;

    cmd_line = malloc(sizeof(char) * SH_CMD_MAX);
    if (cmd_line == NULL) return ERR_MEMORY;

    if ((rc = init_cmd_buff(&cmd)) != OK) return rc;

    while (1) {
      printf("%s", SH_PROMPT);

      if (fgets(cmd_line, ARG_MAX, stdin) == NULL) {
        printf("\n");
        break;
      }

      cmd_line[strcspn(cmd_line, "\n")] = '\0';

      rc = build_cmd_buff(cmd_line, &cmd);
      if (rc == WARN_NO_CMDS) {
        printf(CMD_WARN_NO_CMD);
      }
      if (rc != OK) continue;

      Built_In_Cmds cmd_type = match_command(cmd.argv[0]);
      
      if (cmd_type == BI_NOT_BI) rc = exec_cmd(&cmd);
      else exec_built_in_cmd(&cmd);
    }

    return rc;
}
