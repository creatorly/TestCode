
#ifndef __EOS_POPEN_H
#define __EOS_POPEN_H

#if defined(__cplusplus)
extern "C" {
#endif

/* the same implement with the popen() and pclose() */
FILE* eos_popen(const char *cmdstring, const char *type);
int eos_pclose(FILE *fp);

/* send a signal to the child process */
int eos_pkill(FILE *fp, int sig);

/******************************************************************
 * RUN a shell command in a specified script daemon
 * @param cmd:      The command to be run
 * @param spipe:    The server pipe to which the cmmand is sent
 *
 * @return:         The fd used to read the command output if successfully, 
 *                  -1 otherwise
 */
int eos_fd_popen(const char* cmd, const char* spipe);

#if defined(__cplusplus)
}
#endif

#endif

