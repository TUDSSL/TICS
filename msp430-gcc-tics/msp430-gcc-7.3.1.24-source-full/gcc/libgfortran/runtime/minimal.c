/* Copyright (C) 2002-2017 Free Software Foundation, Inc.
   Contributed by Andy Vaught and Paul Brook <paul@nowt.org>

This file is part of the GNU Fortran runtime library (libgfortran).

Libgfortran is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

Libgfortran is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#include "libgfortran.h"
#include <string.h>


#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* Stupid function to be sure the constructor is always linked in, even
   in the case of static linking.  See PR libfortran/22298 for details.  */
void
stupid_function_name_for_static_linking (void)
{
  return;
}

options_t options;

/* This will be 0 for little-endian
   machines and 1 for big-endian machines.

   Currently minimal libgfortran only runs on little-endian devices
   which don't support constructors so this is just a constant.  */
int big_endian = 0;

static int argc_save;
static char **argv_save;

/* recursion_check()-- It's possible for additional errors to occur
 * during fatal error processing.  We detect this condition here and
 * exit with code 4 immediately. */

#define MAGIC 0x20DE8101

static void
recursion_check (void)
{
  static int magic = 0;

  /* Don't even try to print something at this point */
  if (magic == MAGIC)
    sys_abort ();

  magic = MAGIC;
}


/* os_error()-- Operating system error.  We get a message from the
 * operating system, show it and leave.  Some operating system errors
 * are caught and processed by the library.  If not, we come here. */

void
os_error (const char *message)
{
  recursion_check ();
  printf ("Operating system error: ");
  printf ("%s\n", message);
  exit (1);
}
iexport(os_error);


/* void runtime_error()-- These are errors associated with an
 * invalid fortran program. */

void
runtime_error (const char *message, ...)
{
  va_list ap;

  recursion_check ();
  printf ("Fortran runtime error: ");
  va_start (ap, message);
  vprintf (message, ap);
  va_end (ap);
  printf ("\n");
  exit (2);
}
iexport(runtime_error);

/* void runtime_error_at()-- These are errors associated with a
 * run time error generated by the front end compiler.  */

void
runtime_error_at (const char *where, const char *message, ...)
{
  va_list ap;

  recursion_check ();
  printf ("%s", where);
  printf ("\nFortran runtime error: ");
  va_start (ap, message);
  vprintf (message, ap);
  va_end (ap);
  printf ("\n");
  exit (2);
}
iexport(runtime_error_at);


void
runtime_warning_at (const char *where, const char *message, ...)
{
  va_list ap;

  printf ("%s", where);
  printf ("\nFortran runtime warning: ");
  va_start (ap, message);
  vprintf (message, ap);
  va_end (ap);
  printf ("\n");
}
iexport(runtime_warning_at);


/* void internal_error()-- These are this-can't-happen errors
 * that indicate something deeply wrong. */

void
internal_error (st_parameter_common *cmp, const char *message)
{
  recursion_check ();
  printf ("Internal Error: ");
  printf ("%s", message);
  printf ("\n");

  /* This function call is here to get the main.o object file included
     when linking statically. This works because error.o is supposed to
     be always linked in (and the function call is in internal_error
     because hopefully it doesn't happen too often).  */
  stupid_function_name_for_static_linking();

  exit (3);
}


/* Set the saved values of the command line arguments.  */

void
set_args (int argc, char **argv)
{
  argc_save = argc;
  argv_save = argv;
}
iexport(set_args);


/* Retrieve the saved values of the command line arguments.  */

void
get_args (int *argc, char ***argv)
{
  *argc = argc_save;
  *argv = argv_save;
}

/* sys_abort()-- Terminate the program showing backtrace and dumping
   core.  */

void
sys_abort (void)
{
  /* If backtracing is enabled, print backtrace and disable signal
     handler for ABRT.  */
  if (options.backtrace == 1
      || (options.backtrace == -1 && compile_options.backtrace == 1))
    {
      printf ("\nProgram aborted.\n");
    }

  abort();
}
