#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/videodev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <pthread.h>

#include "../../mjpg_streamer.h"
#include "../../utils.h"
#include "../output.h"
#include "httpd.h"

#define OUTPUT_PLUGIN_NAME "HTTP output plugin"
#define MAX_ARGUMENTS 32

pthread_t   server;
globals     *global;

extern int  port;

/******************************************************************************
Description.: 
Input Value.: 
Return Value: 
******************************************************************************/
void help(void) {
  fprintf(stderr, " ---------------------------------------------------------------\n" \
                  " Help for output plugin..: "OUTPUT_PLUGIN_NAME"\n" \
                  " ---------------------------------------------------------------\n" \
                  " The following parameters can be passed to this plugin:\n\n" \
                  " [-p | --port ]..........: TCP port for this HTTP server\n" \
                  " ---------------------------------------------------------------\n");
}

/*** plugin interface functions ***/
/******************************************************************************
Description.: 
Input Value.: 
Return Value: 
******************************************************************************/
int output_init(output_parameter *param) {
  char *argv[MAX_ARGUMENTS]={NULL};
  int argc=1, i;

  port = htons(8080);

  /* convert the single parameter-string to an array of strings */
  argv[0] = OUTPUT_PLUGIN_NAME;
  if ( param->parameter_string != NULL && strlen(param->parameter_string) != 0 ) {
    char *arg, *saveptr, *token;

    arg=(char *)strdup(param->parameter_string);

    if ( strchr(arg, ' ') != NULL ) {
      token=strtok_r(arg, " ", &saveptr);
      if ( token != NULL ) {
        argv[argc] = strdup(token);
        argc++;
        while ( (token=strtok_r(NULL, " ", &saveptr)) != NULL ) {
          argv[argc] = strdup(token);
          argc++;
          if (argc >= MAX_ARGUMENTS) {
            OPRINT("ERROR: too many arguments to output plugin\n");
            return 1;
          }
        }
      }
    }
  }

  /* show all parameters for DBG purposes */
  for (i=0; i<argc; i++) {
    DBG("argv[%d]=%s\n", i, argv[i]);
  }

  reset_getopt();
  while(1) {
    int option_index = 0, c=0;
    static struct option long_options[] = \
    {
      {"h", no_argument, 0, 0},
      {"help", no_argument, 0, 0},
      {"p", required_argument, 0, 0},
      {"port", required_argument, 0, 0},
      {0, 0, 0, 0}
    };

    c = getopt_long_only(argc, argv, "", long_options, &option_index);

    /* no more options to parse */
    if (c == -1) break;

    /* unrecognized option */
    if (c == '?'){
      help();
      return 1;
    }

    switch (option_index) {
      /* h, help */
      case 0:
      case 1:
        DBG("case 0,1\n");
        help();
        return 1;
        break;

      /* p, port */
      case 2:
      case 3:
        DBG("case 2,3\n");
        port = htons(atoi(optarg));
        break;
    }
  }

  global = param->global;

  OPRINT("HTTP TCP port.....: %d\n", ntohs(port));
  return 0;
}

/******************************************************************************
Description.: 
Input Value.: 
Return Value: 
******************************************************************************/
int output_stop(void) {
  DBG("will cancel server thread\n");
  pthread_cancel(server);
  return 0;
}

/******************************************************************************
Description.: 
Input Value.: 
Return Value: 
******************************************************************************/
int output_run(void) {
  DBG("launching server thread\n");
  pthread_create(&server, 0, server_thread, NULL);
  pthread_detach(server);
  return 0;
}
