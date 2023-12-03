#define _XOPEN_SOURCE 700
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <fcntl.h>
#include <spawn.h>
#include <termios.h>
#include <unistd.h>
#include <wait.h>

int open_pty_pair(int *amaster, int *aslave)
{
  int master, slave;
  char *name;

  master = posix_openpt(O_RDWR | O_NOCTTY);
  if (master < 0)
    return 0;

  if (grantpt(master) < 0 || unlockpt(master) < 0)
  {
    close(master);
    return 0;
  }

  name = ptsname(master);
  if (name == NULL)
  {
    close(master);
    return 0;
  }

  slave = open(name, O_RDWR);
  if (slave == -1)
  {
    close(master);
    return 0;
  }
  return 1;
}

int main(void)
{
  int master, slave;
  int flag = open_pty_pair(&master, &slave);
  if (flag == 0)
  {
    perror("failed to create master, slave");
    return 0;
  }
  printf("master_fd: %d, slave_id: %d\n", master, slave);

  pid_t pid;

  // fork child
  pid = fork();
  if (pid == -1)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  else if (pid == 0)
  { // child process
    close(master);
    dup2(STDIN_FILENO, slave);
    dup2(STDOUT_FILENO, slave);

    posix_spawn_file_actions_t fd_actions;
    posix_spawnattr_t attrs;
    posix_spawnattr_init(&attrs);
    pid_t pid;
    extern char **environ;
    char *const spawn_argv[] = {"bc" , NULL};
    posix_spawnp(&pid, "bc", &fd_actions, &attrs, spawn_argv, environ);
  }
  else
  {
    close(slave);
    dup2(master, STDIN_FILENO);
    dup2(master, STDOUT_FILENO);
  }

  wait(NULL);
  close(master);
  return 0;
}