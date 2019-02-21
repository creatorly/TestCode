
#ifndef __EOS_SIGNAL_H
#define __EOS_SIGNAL_H

#if defined(__cplusplus)
extern "C" {
#endif

#define EOS_SIGMAX  64

typedef struct tagstEosSigSet
{
    int maxsig;
    int mask[(EOS_SIGMAX+(8*sizeof(int))-1)/(8*sizeof(int))];
}eos_sigset_t;

typedef void (*EOS_SIGHANDLER_FP)(int sig, void* data);

int eos_sigemptyset(eos_sigset_t* set);
int eos_sigaddset(eos_sigset_t* set, int sig);
int eos_sigismember(eos_sigset_t* set, int sig);

/* start the signal listen thread in main thread */
int eos_sigstart(eos_sigset_t* set, const char* pidfile);
/* stop the signal main thread and unblock the signals blocked by eos_sigstart() */
int eos_sigstop();
/* only unblock the signals blocked by eos_sigstart(), only used in child process */
int eos_sigclear();

/* install a signal handler for a thread */
/* Note: the signal must be in the sigset in eos_sigstart() */
int eos_signal(int sig, EOS_SIGHANDLER_FP handler, void* data);

/* wait some signals in a thread */
/* Note: the signal must be in the sigset in eos_sigstart() */
int eos_sigwait(eos_sigset_t* set, int* sig);
int eos_sigtimedwait(eos_sigset_t* set, int* sig, int ms);

/* send a signal to the eos signal thread */
int eos_sigkill(int sig);

#if defined(__cplusplus)
}
#endif

#endif

