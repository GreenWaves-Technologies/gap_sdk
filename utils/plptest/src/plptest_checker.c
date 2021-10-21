#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/prctl.h>
#include <signal.h>

static void exit_handler() {
  killpg(0, SIGKILL);
  exit(1);
}

int main(int argc, char **argv) {

  signal(SIGINT, exit_handler);
  signal(SIGKILL, exit_handler);
  signal(SIGSTOP, exit_handler);
  signal(SIGTERM, exit_handler);
  signal(SIGHUP, exit_handler);
  prctl(PR_SET_PDEATHSIG, SIGHUP);

  int pid;
  if ((pid = fork()) == 0) {
    argv++;
    execvp (argv[0], argv);
  } else {

    int status;
    if (waitpid(pid, &status, 0) == -1) return -1;
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
  }
}
