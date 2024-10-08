#if !defined(SPSSS_H)
#define SPSSS_H

#include "spsss-datastruct.h"

#define LINE_MAX 1024
#define SUCCESSFUL_COMPILATION 1
#define FAILED_COMPILATION 0
#define FILE_FLAGS (O_WRONLY | O_CREAT | O_TRUNC)
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

Spsss_commands read_spsss_commands(const char compile_cmds[],
                                   const char test_cmds[]);
void clear_spsss_commands(Spsss_commands *const commands);
int compile_program(Spsss_commands commands);
int test_program(Spsss_commands commands);

#endif
