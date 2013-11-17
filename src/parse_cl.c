/******************************************************************************
**
** parse_cl.c
**
** Tue Nov 12 05:20:54 2013
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
  {"hex", no_argument, NULL, 'x'},
  {"binary", no_argument, NULL, 'b'},
  {"bit-coin", no_argument, NULL, 'c'},
  {"nonce", no_argument, NULL, 'n'},
  {"print-bytes", required_argument, NULL, 'p'},
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
  my_args->x = false;
  my_args->b = false;
  my_args->c = false;
  my_args->n = false;

  optind = 0;
  while ((c = getopt_long (argc, argv, "hvxbcnp:", long_options, &optind)) != - 1)
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

        case 'x':
          my_args->x = true;
          break;

        case 'b':
          my_args->b = true;
          break;

        case 'c':
          my_args->c = true;
          break;

        case 'n':
          my_args->n = true;
          break;

        case 'p':
          my_args->p = atoi (optarg);
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
  -x, --hex               print result in hex string\n\
  -b, --binary            print result as binary values\n\
  -c, --bit-coin          treat file as json bitcoin block and double hash it\n\
  -n, --nonce             if bitcoin then iterate over nonce\n\
  -p, --print-bytes       number of bytes to print\n\
\n", program_name);
    }
  exit (status);
}
