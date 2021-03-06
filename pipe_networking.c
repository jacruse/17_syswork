#include "pipe_networking.h"

/*=========================
  server_setup
  args:

  creates the WKP (upstream) and opens it, waiting for a
  connection.

  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  int wkp;

  printf("[server]: setting up wkp\n");
  //sets up wkp and blocks until sent data from client
  if (mkfifo("well_known_pipe", 0666) == -1) {
    printf("1: %s\n", strerror(errno));
    exit(0);
  }
  if ((wkp = open("well_known_pipe", O_RDONLY)) == -1) {
    printf("2: %s\n", strerror(errno));
    exit(0);
  }
  remove("well_known_pipe");

  return wkp;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  char buffer[BUFFER_SIZE];
  int fifo;

  printf("[server]: setting up wkp\n");
  read(from_client, buffer, BUFFER_SIZE);
  printf("[server]: read %s\n", buffer);

  fifo = open(buffer, O_WRONLY, 0);
  write(fifo, "init", sizeof(char *));

  if (read(from_client, buffer, BUFFER_SIZE)) {
      printf("[server]: connection established\n");
  }
  return fifo;
}

/*=========================
  server_handshake
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {

  int from_client;

  char buffer[HANDSHAKE_BUFFER_SIZE];

  mkfifo("well_known_pipe", 0600);

  //block on open, recieve mesage
  printf("[server] handshake: making wkp\n");
  from_client = open( "well_known_pipe", O_RDONLY, 0);
  read(from_client, buffer, sizeof(buffer));
  printf("[server] handshake: received [%s]\n", buffer);

  remove("well_known_pipe");
  printf("[server] handshake: removed wkp\n");

  //connect to client, send message
  *to_client = open(buffer, O_WRONLY, 0);
  write(*to_client, buffer, sizeof(buffer));

  //read for client
  read(from_client, buffer, sizeof(buffer));
  printf("[server] handshake received: %s\n", buffer);

  return from_client;
}

/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {

  int from_server;
  char buffer[HANDSHAKE_BUFFER_SIZE];
  char fifo_name[HANDSHAKE_BUFFER_SIZE];

  //send pp name to server
  printf("[client] handshake: connecting to wkp\n");
  *to_server = open( "well_known_pipe", O_WRONLY, 0);
  if ( *to_server == -1 )
    exit(1);

  //make private pipe
  sprintf(fifo_name, "%d", getpid() );
  //printf("%s\n", buffer);
  mkfifo(fifo_name, 0600);

  write(*to_server, fifo_name, sizeof(buffer));

  //open and wait for connection
  from_server = open(fifo_name, O_RDONLY, 0);
  read(from_server, buffer, sizeof(buffer));
  /*validate buffer code goes here */
  printf("[client] handshake: received [%s]\n", buffer);

  //remove pp
  if (remove(fifo_name) == -1) {
    printf("%s\n", strerror(errno));
    exit(0);
  }
  printf("[client] handshake: removed pp\n");

  //send ACK to server
  write(*to_server, ACK, sizeof(buffer));

  return from_server;
}
