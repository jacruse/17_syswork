#include "pipe_networking.h"
#include <signal.h>

void process(char *s);
void subserver(int from_client);

static void sighandler(int signo) {
  if (signo == SIGINT) {
    remove("well_known_pipe");
    exit(0);
  }
}

int main() {
  int from_client;
  int to_client;
  signal(SIGINT, sighandler);

  while(1) {
    from_client = server_setup();
    while (read(from_client, ))



  }
}

void subserver(int from_client) {
}

void process(char * s) {

}
