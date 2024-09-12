#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "spsss.h"
#include "safe-fork.h"
#include "split.h"



static void read_cmd(const char* name, char *** commands, unsigned int* count);

static void read_cmd(const char* name, char *** commands, unsigned int* count) {
  char line[LINE_MAX+1];
  int i = 0;
  FILE* f = fopen(name, "r");
  *commands = NULL;
  *count = 0;
  if (f == NULL) {
    perror("error opening file");
    exit(1);
  }
    
  while (fgets(line, sizeof(line), f) != NULL) {
    *commands = realloc(*commands, ((*count) + 1) * sizeof(char*));
    if (*commands == NULL) {
      perror("realloc failed");
      exit(1);
    }
    (*commands)[i] = malloc(strlen(line) + 1);
    if ((*commands)[i] == NULL) {
      perror("malloc failed");
      exit(1);
    }
    strcpy((*commands)[i], line);
    (*count)++;
    i++;
  }

  fclose(f);
  if (*count == 0) {
    exit(1);
  }
} 
    
    
      
  


Spsss_commands read_spsss_commands(const char compile_cmds[],
                                   const char test_cmds[]) {
  Spsss_commands cmds;
  if (compile_cmds == NULL || test_cmds == NULL)
    exit(1);
  
  cmds.compile_commands = NULL;
  cmds.test_commands = NULL;
  cmds.compiled = 0;
  cmds.tested = 0;

  read_cmd(compile_cmds, &cmds.compile_commands, &cmds.compiled);
  read_cmd(test_cmds, &cmds.test_commands, &cmds.tested);
  
  return cmds;
}
void clear_spsss_commands(Spsss_commands *const commands) {
  int i;
  if (commands == NULL)
    return;
  if (commands->compile_commands != NULL) {
  for (i = 0; i < commands->compiled; i++)
    free(commands->compile_commands[i]);
  
  free(commands->compile_commands);
  }
  if (commands->test_commands != NULL) {
  for (i = 0; i < commands->tested; i++)
    free(commands->test_commands[i]);
  
  free(commands->test_commands);
  }

  commands->compiled = 0;
  commands->tested = 0;
  
}

int compile_program(Spsss_commands commands) {
  int i, j, input, output, status, dups;
  char **words; 
  
  if (commands.compiled == 0)
    return SUCCESSFUL_COMPILATION;
  
  for (i = 0; i < commands.compiled; i++) {
    pid_t pid = safe_fork();
    if (pid == 0) {
      
      words = split(commands.compile_commands[i]);
    
      for (j = 0; words[j] != NULL; j++) {
	
	if (strcmp(words[j], "<") == 0) {
	input = open(words[j+1], O_RDONLY);
	if (input == -1) {
	  perror("input failed");
	  exit(1);
	}
    
	dups = dup2(input, STDIN_FILENO);
	if (dups == -1) {
	  perror("dup failed");
	  exit(1);
	}
	close(input);
	words[j+1] = NULL;
	words[j] = NULL;	     

	j++;	     
	}
        else if (strcmp(words[j], ">") == 0) {
	output = open(words[j+1], FILE_FLAGS, FILE_MODE);
	if (output == -1) {
	  perror("output failed");
	  exit(1);
	}
	dups = dup2(output, STDOUT_FILENO);
	if (dups == -1) {
	  perror("dup failed");
	  exit(1);
	}
	close(output);
	words[j+1] = NULL;
	words[j] = NULL;
	j++;
	}
      }      
      execvp(words[0], words); 
      perror("execvp failed");
      exit(1);
    }
    
  
    else if (pid > 0) {
      waitpid(pid, &status, 0);

      if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	return FAILED_COMPILATION;
    }
    else {
      perror("fork failed");
      exit(1);
    }
  }
  return SUCCESSFUL_COMPILATION;
}

int test_program(Spsss_commands commands) {
  int i, j, input, output, status, dups;
  char **words;
  int successes = 0;
  
  if (commands.tested == 0)
    return 0;
  for (i = 0; i < commands.tested; i++) {
    pid_t pid = safe_fork();
    if (pid == 0) {
      words = split(commands.test_commands[i]);
      for (j = 0; words[j] != NULL; j++) {
	
	if (strcmp(words[j], "<") == 0) {
	  input = open(words[j+1], O_RDONLY);
	  if (input == -1) {
	    perror("input failed");
	    exit(1);
	  }
    
	  dups = dup2(input, STDIN_FILENO);
	  if (dups == -1) {
	    perror("dup failed");
	    exit(1);
	  }
	  close(input);
	  words[j+1] = NULL;
	  words[j] = NULL;
	  j++;
	}
        else if (strcmp(words[j], ">") == 0) {
	  output = open(words[j+1], FILE_FLAGS, FILE_MODE);
	  if (output == -1) {
	    perror("output failed");
	    exit(1);
	  }
	  dups = dup2(output, STDOUT_FILENO);
	  if (dups == -1) {
	    perror("dup failed");
	    exit(1);
	  }
	  close(output);
	  words[j+1] = NULL;
	  words[j] = NULL;
	  j++;
	}
      }
    
      execvp(words[0], words);
	
      perror("execvp failed");
      exit(1);

    }
      
    else if (pid > 0) {
      waitpid(pid, &status, 0);

      if (WIFEXITED(status) && WEXITSTATUS(status) == 0) 
        successes++;
      
    }
    else {
      perror("fork failed");
      exit(1);
    }
  }
  return successes;
}























