#include "pipe_networking.h"
#include <signal.h>
#include <ctype.h>

void subserver(int from_client);
void process(char *s);

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("well_known_pipe");
    exit(0);
  }
}

int main() {
  int from_client;
  int f;

  if (signal(SIGINT, sighandler) == SIG_ERR) {
    printf("Can't catch SIGINT\n");
  }

  while(1) {
    from_client = server_setup();
    f = fork();
    if (!f) {
      subserver(from_client);
    }
  }
}

void subserver(int from_client) {
  char buffer [BUFFER_SIZE];
  int to_client;

  to_client = server_connect(from_client);

  while (read(from_client, buffer, BUFFER_SIZE)) {
    printf("[subserver %d]: received %s\n", getpid(), buffer);
    process(buffer);
    printf("[subserver %d]: sending %s\n", getpid(), buffer);
    write(to_client, buffer, BUFFER_SIZE);
  }

  exit(0);
}

void process(char * s) {
  //change to cAmElCaSe
  unsigned int i;
  for (i = 0; i < strlen(s); i ++) {
    if (i % 2) {
      s[i] = toupper(s[i]);
    }
    else {
      s[i] = tolower(s[i]);
    }
  }
}
