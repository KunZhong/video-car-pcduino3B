#ifndef _UVC_HTTPD_H
#define _UVC_HTTPD_H

struct buff {
  int size;
  unsigned char *buff;
};

struct thread_buff {
  pthread_mutex_t lock;
  pthread_cond_t  cond;
  cqueue_t qbuff;
};

/* client thread type */
typedef void *(*client_thread_t)(void *);

struct http_server {
  int sd;
  int port;
  struct thread_buff *ptbuff;
  pthread_t client;
  client_thread_t client_thread;
} server;

typedef enum { UNKNOWN, INVALID, SNAPSHOT, STREAM } request_t;

struct clientArgs {
  int socket;
  struct sockaddr_in client_addr;
  struct http_server *server;
  request_t request_type;
};

int http_listener(struct http_server *srv);

#endif
