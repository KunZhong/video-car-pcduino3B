#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include "cqueue.h"
#include "http.h"

#define BUFF_MAX      1024
#define READ_TIMEOUT  30

extern int stop;

struct http_header {
  char *method;
  char *uri;
};

static void http_header_free(struct http_header *header);

static int data_available(int socket, int timeout)
{
  struct timeval to;
  fd_set fds;
  to.tv_sec = timeout, to.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(socket, &fds);
  return select(socket+1, &fds, NULL, NULL, &to);
}

static int http_header_readline(int fd, char *buf, int len)
{
  char *ptr = buf;
  char *ptr_end = ptr + len - 1;

  if(data_available(fd, READ_TIMEOUT) <= 0) {
    return -1;
  }

  while (ptr < ptr_end) {
    switch (read(fd, ptr, 1)) {
    case 1:
	if(*ptr == '\n'){
		*ptr = '\0';
		return ptr - buf;
	}else{
       		ptr++;
		continue;
	}
       break;
   case 0:
      *ptr = '\0';
      return ptr - buf;
    default:
      printf("%s() failed: %s\n", __func__, strerror(errno));
      return -1;
    }
  }

  return len;
}

static int http_parse_header(struct clientArgs *client, struct http_header *header)
{
  char header_line[BUFF_MAX];
  char *token = NULL;
  int res;

  header->method= NULL;
  header->uri = NULL;

  client->request_type = UNKNOWN;

  /* fcntl(socketfd, F_SETFL, fcntl(socketfd, F_GETFL, 0) | O_NONBLOCK);*/
  if((res = http_header_readline( client->socket, header_line, sizeof(header_line))) > 0) {
      token = strtok(header_line, "/");
      if(token) {
        header->method = strdup(token);
      }
      token = strtok(NULL, "/");
      if(token) {
        header->uri = strdup(token);
      }
  }

  if (header->uri) {
    if (!strcmp(header->uri, SNAPSHOT_URI)) {
      client->request_type = SNAPSHOT;
    }
    if (!strcmp(header->uri, STREAM_URI)) {
      client->request_type = STREAM;
    }
    if (!strcmp(header->uri, STREAM_URI)) {
      client->request_type = STREAM;
    }
    if (!strcmp(header->uri, STREAM_URI)) {
      client->request_type = STREAM;
    }
     if (!strcmp(header->uri, STREAM_URI)) {
      client->request_type = STREAM;
    }
 
  } else {
    client->request_type= INVALID;
  }
  return res;
}

static void http_header_free(struct http_header *header)
{
  if(header->method){
    free(header->method);
  }

  if(header->uri){
    free(header->uri);
  }
}

int http_listener(struct http_server *srv,client_thread_t client_thread)
{
  struct sockaddr_in addr;
  int on=1;
  int c = sizeof(struct sockaddr_in);
  struct clientArgs *ca;
  /* open socket for server */
  srv->sd = socket(PF_INET, SOCK_STREAM, 0);
  if ( srv->sd < 0 ) {
    fprintf(stderr, "socket failed\n");
    exit(1);
  }

  /* ignore "socket already in use" errors */
  if (setsockopt(srv->sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    perror("setsockopt(SO_REUSEADDR) failed");
    exit(1);
  }
/*
  if(setsockopt( srv->sd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0) {
    printf("Couldn't setsockopt(TCP_NODELAY)\n");
    exit(-1);
  }
*/
 /* configure server address to listen to all local IPs */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = srv->port;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if ( bind(srv->sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ) {
    perror("Bind");
    exit(1);
  }

  /* start listening on socket */
  if (listen(srv->sd, 10) != 0 ) {
    fprintf(stderr, "listen failed\n");
    exit(1);
  }

  srv->client_thread = client_thread;
  while( 1 ) {
    /* alloc new client */
    ca = malloc(sizeof(struct clientArgs));
    ca->server = srv;
    ca->socket = accept(srv->sd, (struct sockaddr *)&ca->client_addr, (socklen_t*)&c);
    if (ca->socket < 0) {
      perror("accept failed");
      free(ca);
      continue;
    }

    if( pthread_create(&srv->client, NULL,  srv->client_thread, ca) < 0) {
      perror("could not create client thread");
      return 1;
    }
  }
}
