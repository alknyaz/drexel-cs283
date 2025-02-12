#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

int build_cmd(char *cmd_str, command_t *cmd) {
  char *token;

  if ((token = strtok(cmd_str, SPACE_STRING)) == NULL) return WARN_NO_CMDS;
  if (strlen(token) >= EXE_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
  strcpy(cmd->exe, token);

  cmd->args[0] = '\0';
  if ((token = strtok(NULL, SPACE_STRING)) == NULL) return OK;
  if (strlen(token) >= ARG_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;
  strcpy(cmd->args, token);

  while ((token = strtok(NULL, SPACE_STRING)) != NULL)  {
    int token_len = strlen(token);
    int args_len = strlen(cmd->args);
    if (token_len + args_len + 1 >= ARG_MAX) return ERR_CMD_OR_ARGS_TOO_BIG;

    cmd->args[args_len] = SPACE_CHAR;
    strcpy(cmd->args + args_len + 1, token);
  }

  return OK;
}

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    clist->num = 0;

    char *token;
    char *rest = cmd_line;
    int err;

    token = strtok_r(cmd_line, PIPE_STRING, &rest);

    while (token != NULL) {
      if (clist->num >= CMD_MAX) return ERR_TOO_MANY_COMMANDS;
      if ((err = build_cmd(token, clist->commands + clist->num)) != OK) return err;
      clist->num++;
      token = strtok_r(NULL, PIPE_STRING, &rest);
    }

    if (clist->num == 0) return WARN_NO_CMDS;
    return OK;
  }
