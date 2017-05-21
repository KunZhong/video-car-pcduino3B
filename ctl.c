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
#include "ctl.h"
#include "http.h"
#include "driver.h"

#define GRAVITY_ON_CMD "gravityon"
#define GRAVITY_OFF_CMD "gravityoff"

#define LIGHT_ON_CMD "lighton"
#define LIGHT_OFF_CMD "lightoff"
#define GO_CMD "go"
#define BACK_CMD "back"	
#define LEFT_CMD "left"
#define RIGHT_CMD "right"
#define STOP_CMD  "stop"

#define CAMERA_UP_CMD  "camup"
#define CAMERA_DOWN_CMD  "camdown"
#define BUFF_MAX      1024
#define READ_TIMEOUT  30

struct http_header {
  char *method;
  char *uri;
};

static void http_header_free(struct http_header *header);

static int http_header_readline(int fd, char *buf, int len)
{
  char *ptr = buf;
  char *ptr_end = ptr + len - 1;

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
    if (!strcmp(header->uri, GO_CMD)) {
      client->request_type = GO;
    }
    if (!strcmp(header->uri, STOP_CMD)) {
      client->request_type = STOP;
    }
    if (!strcmp(header->uri, BACK_CMD)) {
      client->request_type = BACK;
    }
    if (!strcmp(header->uri, LEFT_CMD)) {
      client->request_type = LEFT;
    }
    if (!strcmp(header->uri, RIGHT_CMD)) {
      client->request_type = RIGHT;
    }
    if (!strcmp(header->uri, GRAVITY_ON_CMD)) {
      client->request_type = GRAVITY_ON;
    }
    if (!strcmp(header->uri, GRAVITY_OFF_CMD)) {
      client->request_type = GRAVITY_OFF;
    }
    if (!strcmp(header->uri, LIGHT_ON_CMD)) {
      client->request_type = LIGHT_ON;
    }
    if (!strcmp(header->uri, LIGHT_OFF_CMD)) {
      client->request_type = LIGHT_OFF;
    }
    if (!strcmp(header->uri, CAMERA_UP_CMD)) {
      client->request_type = CAMERA_UP;
    }
    if (!strcmp(header->uri, CAMERA_DOWN_CMD)) {
      client->request_type = CAMERA_DOWN;
    }
  }
   else {
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

/* thread for clients that connected to this server */
void *ctl_client_thread( void *arg)
{
  struct clientArgs *ca = (struct clientArgs *)arg;
  int should_close_connection = 0;
  struct http_header header;
  char buffer[1024];

  pthread_detach(pthread_self());

  printf("ctl_thread_id: %ld request %s\n", pthread_self(), header.uri);

  while(1)
  {
    if(http_parse_header(ca, &header) < 0){
	printf("close ctl socket!\n");
      close(ca->socket);
      http_header_free(&header);
      free(arg);
      return NULL;
    }

    switch (ca->request_type) {
      case GO:
        snprintf(buffer, sizeof(buffer),"go");
	go();
      break;
      case STOP:
        snprintf(buffer, sizeof(buffer),"stop");
	Car_Stop();
      break;
      case BACK:
        snprintf(buffer, sizeof(buffer),"back");
	back();
      break;
      case LEFT:
        snprintf(buffer, sizeof(buffer),"left");
	left();
      break;
      case RIGHT:
        snprintf(buffer, sizeof(buffer),"right");
	right();
      break;
      case GRAVITY_ON:
        snprintf(buffer, sizeof(buffer),"gravityon");
      break;
      case GRAVITY_OFF:
        snprintf(buffer, sizeof(buffer),"gravityoff");
      break;
      case LIGHT_ON:
        snprintf(buffer, sizeof(buffer),"lighton");
      break;
      case LIGHT_OFF:
        snprintf(buffer, sizeof(buffer),"lightoff");
      break;
      case CAMERA_UP:
        snprintf(buffer, sizeof(buffer),"camera_up");
	camera_up();
      break;
      case CAMERA_DOWN:
        snprintf(buffer, sizeof(buffer),"camera_down");
	camera_down();
      break;
     default:
        snprintf(buffer, sizeof(buffer),"The requested URL %s was not found on this server\n", header.uri);
        should_close_connection = 1;
      break;
    }
    printf("cmd: %s\n",buffer);
    if (should_close_connection) {
      close(ca->socket);
      http_header_free(&header);
      free(arg);
      return NULL;
    }
  }
  close(ca->socket);
  http_header_free(&header);
  free(arg);
  return NULL;
}

