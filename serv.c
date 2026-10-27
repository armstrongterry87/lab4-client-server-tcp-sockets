#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include "list.h"

#define PORT 9001
#define ACK "ACK"
#define BUF_LEN 1024

// globals so the signal handler can clean up
static list_t *mylist = NULL;
static int servSockD = -1;
static int clientSocket = -1;

static void cleanup(void) {
  if (mylist != NULL) { list_free(mylist); mylist = NULL; }
  if (clientSocket >= 0) { close(clientSocket); clientSocket = -1; }
  if (servSockD >= 0) { close(servSockD); servSockD = -1; }
}

// Ctrl-C: free the list, close sockets, exit gracefully
static void sigint_handler(int sig) {
  (void)sig;
  printf("\nServer shutting down...\n");
  cleanup();
  exit(0);
}

// returns next space-separated token as int, or sets *ok = 0 if missing
static int next_int(int *ok) {
  char *t = strtok(NULL, " ");
  if (t == NULL) { *ok = 0; return 0; }
  return atoi(t);
}

int main(int argc, char const* argv[])
{
  int n, val, idx, ok;

  // string store data to recv/send to/from client
  char buf[BUF_LEN];
  char sbuf[BUF_LEN];
  char* token;

  signal(SIGINT, sigint_handler);

  // create server socket
  servSockD = socket(AF_INET, SOCK_STREAM, 0);
  if (servSockD < 0) { perror("socket"); exit(1); }

  // allow quick restarts (avoid "address already in use")
  int opt = 1;
  setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  // define server address
  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(PORT);
  servAddr.sin_addr.s_addr = INADDR_ANY;

  // bind socket to the specified IP and port
  if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
    perror("bind");
    cleanup();
    exit(1);
  }

  // listen for connections
  listen(servSockD, 1);

  // integer to hold client socket.
  clientSocket = accept(servSockD, NULL, NULL);
  if (clientSocket < 0) { perror("accept"); cleanup(); exit(1); }

  mylist = list_alloc();  // create the list

  while (1) {
    // recvs messages from client socket
    n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
    if (n <= 0) break;            // client disconnected
    buf[n] = '\0';

    memset(sbuf, '\0', BUF_LEN);
    ok = 1;
    token = strtok(buf, " ");
    if (token == NULL) token = "";

    if (strcmp(token, "exit") == 0) {
      cleanup();
      exit(0);
    }
    else if (strcmp(token, "print") == 0) {
      char *s = listToString(mylist);
      snprintf(sbuf, BUF_LEN, "%s", s);
      free(s);
    }
    else if (strcmp(token, "get_length") == 0) {
      val = list_length(mylist);
      snprintf(sbuf, BUF_LEN, "%s%d", "Length = ", val);
    }
    else if (strcmp(token, "add_back") == 0) {
      val = next_int(&ok);
      if (ok) {
        list_add_to_back(mylist, val);
        snprintf(sbuf, BUF_LEN, "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "add_front") == 0) {
      val = next_int(&ok);
      if (ok) {
        list_add_to_front(mylist, val);
        snprintf(sbuf, BUF_LEN, "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "add_position") == 0) {
      idx = next_int(&ok);
      val = next_int(&ok);
      if (ok) {
        list_add_at_index(mylist, val, idx);
        snprintf(sbuf, BUF_LEN, "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "remove_back") == 0) {
      val = list_remove_from_back(mylist);
      snprintf(sbuf, BUF_LEN, "%s%d", ACK, val);
    }
    else if (strcmp(token, "remove_front") == 0) {
      val = list_remove_from_front(mylist);
      snprintf(sbuf, BUF_LEN, "%s%d", ACK, val);
    }
    else if (strcmp(token, "remove_position") == 0) {
      idx = next_int(&ok);
      if (ok) {
        val = list_remove_at_index(mylist, idx);
        snprintf(sbuf, BUF_LEN, "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "get") == 0) {
      idx = next_int(&ok);
      if (ok) {
        val = list_get_elem_at(mylist, idx);
        snprintf(sbuf, BUF_LEN, "%s%d", "VALUE = ", val);
      }
    }
    else if (strcmp(token, "menu") == 0) {
      snprintf(sbuf, BUF_LEN, "%s", "MENU");
    }
    else {
      snprintf(sbuf, BUF_LEN, "%s", "Invalid command");
    }

    if (!ok) snprintf(sbuf, BUF_LEN, "%s", "Missing argument");

    // send's messages to client socket
    send(clientSocket, sbuf, sizeof(sbuf), 0);
    memset(buf, '\0', BUF_LEN);
  }

  cleanup();
  return 0;
}
