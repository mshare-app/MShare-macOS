//
//  message_client.c
//  MShare
//
//  Created by Jithin Renji on 1/1/24.
//

#include "message_client.h"

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <libproc.h>

int msclient_send_packet(const char *serialized) {
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    return SOCKET_CREATION_ERROR;
  }

  int ret = -1;

  size_t msg_len = strlen(serialized);
  if (msg_len >= 4096) {
    ret = MSG_TOO_LONG_ERROR;
    goto die;
  }

  struct sockaddr_in saddr = {
    .sin_family = AF_INET,
    .sin_port = htons(3000)
  };

  if (inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr) != 1) {
    fprintf(stderr, "Addr parse err.\n");
    ret = ADDR_PARSE_ERROR;
    goto die;
  }

  if (connect(sfd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
    fprintf(stderr, "Connection failed err.\n");
    ret = CONN_FAILED_ERROR;
    goto die;
  }

  char buf[4096];
  memset(buf, 0, 4096);
  strcpy(buf, serialized);
  ssize_t nbytes_sent = send(sfd, serialized, 4096, 0);
  if (nbytes_sent == -1 || nbytes_sent != msg_len) {
    printf("%s\n", nbytes_sent == -1 ? "Senderror" : "");
    ret = PARTIAL_SEND_ERROR;
  }

  ret = 0;

die:
  close(sfd);
  return ret;
}
