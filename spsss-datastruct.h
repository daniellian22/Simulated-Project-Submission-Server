#ifndef SPSSS_DATASTRUCT_H
#define SPSSS_DATASTRUCT_H

typedef struct {
  char** compile_commands;
  char** test_commands;
  unsigned int compiled;
  unsigned int tested;
} Spsss_commands;

#endif 

