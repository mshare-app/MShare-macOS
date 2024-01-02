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

int msclient_send_packet(int sfd, const char *serialized) {
  size_t msg_len = strlen(serialized);
  printf("Got msg_len: %ld\n", msg_len);
  if (msg_len >= 4096) {
    return MSG_TOO_LONG_ERROR;
  }

  struct sockaddr_in saddr = {
    .sin_family = AF_INET,
    .sin_port = htons(3000)
  };

  if (inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr) != 1) {
    return ADDR_PARSE_ERROR;
  }

  ssize_t nbytes_sent = sendto(sfd, serialized, msg_len, 0, (struct sockaddr *) &saddr, sizeof(saddr));
  if (nbytes_sent == -1 || nbytes_sent != msg_len) {
    perror("");
    printf("%s\n", nbytes_sent == -1 ? "Senderror" : "");
    return PARTIAL_SEND_ERROR;
  }

  return 0;
}
