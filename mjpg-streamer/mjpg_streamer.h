/*******************************************************************************
# This package work with the Logitech UVC based webcams with the mjpeg feature.#
#                                                                              #
#     Copyright (C) 2005 2006 Laurent Pinchart &&  Michel Xhaard               #
#     Copyright (C) 2007      Tom Stöveken                                     #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/
#define SOURCE_VERSION "2.0"
#define MAX_OUTPUT_PLUGINS 10

#ifdef DEBUG
#define DBG(...) fprintf(stderr, " DBG(%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

#include "plugins/input.h"
#include "plugins/output.h"

typedef struct {
  int stop;

  /* signal fresh frames */
  pthread_mutex_t db;
  pthread_cond_t  db_update;

  /* global JPG frame, this is more or less the "database" */
  unsigned char *buf;
  int size;

  /* input plugin */
  input in;

  /* output plugin */
  output out[MAX_OUTPUT_PLUGINS];
  int outcnt;

} globals;
