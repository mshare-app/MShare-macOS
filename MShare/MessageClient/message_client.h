//
//  message_client.h
//  MShare
//
//  Created by Jithin Renji on 1/1/24.
//

#ifndef message_client_h
#define message_client_h

#include <stdbool.h>

typedef enum send_error {
  MSG_TOO_LONG_ERROR = 1,
  ADDR_PARSE_ERROR,
  PARTIAL_SEND_ERROR
} send_error_t;

int msclient_send_packet(int sfd, const char *message);
#endif /* message_client_h */
