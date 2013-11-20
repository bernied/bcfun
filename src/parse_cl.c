/******************************************************************************
**
** parse_cl.c
**
** Wed Nov 20 16:14:49 2013
** Linux 3.2.0-23-generic-pae (#36-Ubuntu SMP Tue Apr 10 22:19:09 UTC 2012) i686
** vagrant@precise32 (vagrant)
**
** C file for command line parser
**
** Automatically created by genparse v0.8.7
**
** See http://genparse.sourceforge.net for details and updates
**
******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "parse_cl.h"

static struct option const long_options[] =
{
  {"help", no_argument, NULL, 'h'},
  {"version", no_argument, NULL, 'v'},
  {"bit-coin", no_argument, NULL, 'b'},
  {"mine", no_argument, NULL, 'm'},
  {"start", required_argument, NULL, 's'},
  {"end", required_argument, NULL, 'e'},
  {"print-binary", required_argument, NULL, '0'},
  {"print-hex", no_argument, NULL, 'x'},
  {NULL, 0, NULL, 0}
};

/*----------------------------------------------------------------------------
**
** Cmdline ()
**
** Parse the argv array of command line parameters
**
**--------------------------------------------------------------------------*/

void Cmdline (struct arg_t *my_args, int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int c;
  int errflg = 0;

  my_args->h = false;
  my_args->v = false;
  my_args->b = false;
  my_args->m = false;
  my_args->x = 1;

  optind = 0;
  while ((c = getopt_long (argc, argv, "hvbms:e:0:x", long_options, &optind)) != - 1)
    {
      switch (c)
        {
        case 'h':
          my_args->h = true;
          usage (EXIT_SUCCESS, argv[0]);
          break;

        case 'v':
          my_args->v = true;
          break;

        case 'b':
          my_args->b = true;
          break;

        case 'm':
          my_args->m = true;
          break;

        case 's':
          my_args->s = (unsigned int) atol (optarg);
          break;

        case 'e':
          my_args->e = (unsigned int) atol (optarg);
          break;

        case '0':
          my_args->_0 = atoi (optarg);
          break;

        case 'x':
          my_args->x = true;
          break;

        default:
          usage (EXIT_FAILURE, argv[0]);

        }
    } /* while */

  if (errflg)
    usage (EXIT_FAILURE, argv[0]);

  if (optind >= argc)
    my_args->optind = 0;
  else
    my_args->optind = optind;
}

/*----------------------------------------------------------------------------
**
** usage ()
**
** Print out usage information, then exit
**
**--------------------------------------------------------------------------*/

void usage (int status, char *program_name)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, "Try `%s --help' for more information.\n",
            program_name);
  else
    {
      printf ("\
Usage: %s [OPTION]... [FILE]\n\
\n\
  -h, --help              display this help and exit\n\
  -v, --version           output version information and exit\n\
  -b, --bit-coin          treat file as json bitcoin block and double hash it\n\
  -m, --mine              iterate through all nonces\n\
  -s, --start             starting nonce for mining\n\
  -e, --end               ending nonce for mining\n\
  -0, --print-binary      output num bytes as binary\n\
  -x, --print-hex         print result as hex string (default)\n\
\n", program_name);
    }
  exit (status);
}
