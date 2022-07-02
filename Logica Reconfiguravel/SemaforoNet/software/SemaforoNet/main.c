#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <io.h>
#include "includes.h"
#include "system.h"
#include "dm9000a.h"
#include "simple_socket_server.h"
#include "alt_error_handler.h"
#include "ipport.h"
#include "tcpport.h"
#include "libport.h"
#include "osport.h"
#include "basic_io.h"

#define PORT 5050
#define HOST inet_addr("192.168.0.156")
#define BUFFER_SIZE 33
#define REGISTER_3 3

OS_STK    SSSInitialTaskStk[TASK_STACKSIZE];

int GetRegisterId(char *message) {
    char *copy = malloc(strlen(message));
    int value;

    memset(copy, '\0', strlen(message));
    strncpy(copy, message, 4);
    value = (int) strtol(copy, NULL, 2);
    free(copy);

    return value;
}

int GetRegisterValue(char *message) {
    char *copy = malloc(strlen(message));
    int value;

    memset(copy, '\0', strlen(message));
    strncpy(copy, message + 4, strlen(message) - 4);
    value = (int) strtol(copy, NULL, 2);
    free(copy);

    return value;
}

void SSSInitialTask(void *task_data)
{
	INT8U error_code;

	alt_iniche_init();
	netmain();

	while (!iniche_net_ready)
		TK_SLEEP(1);

	printf("\nClient initializing...\n");

	int client;
	unsigned long timeout = 1;
	bool connected = false;
	struct sockaddr_in address;
	char *request = malloc(BUFFER_SIZE);
	char *response = malloc(BUFFER_SIZE);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = HOST;
	address.sin_port = htons(PORT);

	client = socket(PF_INET, SOCK_STREAM, 0);

	printf("Socket OK.\n");
	if (connect(client, (struct sockaddr *) &address, sizeof address) != -1) {
		printf("Connected\n");
		connected = true;
		while (connected) {
			memset(request, '\0', BUFFER_SIZE);
			memset(response, '\0', BUFFER_SIZE);
			sprintf(response, "%010u", IORD(SEMAFORONET_0_BASE, REGISTER_3));

			if (send(client, response, strlen(response), 0) == -1) {
				connected = false;
				close(client);
				free(request);
				free(response);
				perror("Could not send package.\n");
				exit(EXIT_FAILURE);
			}

			if (recv(client, request, BUFFER_SIZE - 1, 0) > -1) {
                if (strcmp(request, "00000000000000000000000000000000\n") == 1) {
                    IOWR(SEMAFORONET_0_BASE, GetRegisterId(request), GetRegisterValue(request));
                }
            }
			msleep(10);
		}
	} else {
		close(client);
		free(request);
		free(response);
		perror("Could not connect.\n");
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char* argv[], char* envp[])
{

  INT8U error_code;

  DM9000A_INSTANCE( DM9000A_0, dm9000a_0 );
  DM9000A_INIT( DM9000A_0, dm9000a_0 );
  OSTimeSet(0);
  error_code = OSTaskCreateExt(SSSInitialTask,
                             NULL,
                             (void *)&SSSInitialTaskStk[TASK_STACKSIZE],
                             SSS_INITIAL_TASK_PRIORITY,
                             SSS_INITIAL_TASK_PRIORITY,
                             SSSInitialTaskStk,
                             TASK_STACKSIZE,
                             NULL,
                             0);
  alt_uCOSIIErrorHandler(error_code, 0);
  OSStart();

  while(1);

  return -1;
}
