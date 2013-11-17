/******************************************************************************
**
** parse_cl.h
**
** Tue Nov 12 05:20:54 2013
** Linux 3.2.0-23-generic-pae (#36-Ubuntu SMP Tue Apr 10 22:19:09 UTC 2012) i686
** vagrant@precise32 (vagrant)
**
** Header file for command line parser
**
** Automatically created by genparse v0.8.7
**
** See http://genparse.sourceforge.net for details and updates
**
******************************************************************************/

#include <stdio.h>

#ifndef bool
typedef enum bool_t
{
  false = 0, true
} bool;
#endif

/* customized structure for command line parameters */
struct arg_t
{
  bool h;
  bool v;
  bool x;
  bool b;
  bool c;
  bool n;
  int p;
  int optind;
};

/* function prototypes */
void Cmdline (struct arg_t *my_args, int argc, char *argv[]);
void usage (int status, char *program_name);
