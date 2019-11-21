#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "gimple-expr.h"
#include "df.h"
#include "tm_p.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "calls.h"
#include "output.h"
#include "explow.h"
#include "expr.h"
#include "langhooks.h"
#include "builtins.h"
#include "intl.h"
#include "msp430-devices.h"

struct t_msp430_mcu_data extracted_mcu_data;

/* If modifying this string check uses of it in this file where characters at
   the start are skipped.  */
static const char * advice_string
  = "Please obtain the latest version of the msp430-gcc-support-files archive "
  "from: \n\"http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/index_FDS.html\"\n"
  "and place the full path to the \"include\" directory containing devices.csv "
  "on an include path specified with -I\n"
  "Defaulting to the hard-coded device data...";

/* Give function unique name in case some system headers e.g. libgen.h are included. */
char *
msp430_dirname (char *path)
{
  int i;
  int len = strlen (path);
  /* Start from the second last char in case the string ends with a dirsep.  */
  for (i = len - 2; i >= 0; i--)
    {
      if (IS_DIR_SEPARATOR (path[i]))
	{
	  path[i] = '\0';
	  return path;
	}
    }
  return path;
}

/* This is to canonicalize the path. On windows we can have a mix of forward
   and backslashes which means the path passed to -L is bogus.  */
static void
canonicalize_path_dirsep (char **path)
{
  char *t_path = *path;
  unsigned int i;
  for (i = 0; i < strlen (t_path); i++)
    if (IS_DIR_SEPARATOR (t_path[i]))
      t_path[i] = DIR_SEPARATOR;
}

void
process_collect_gcc_into_devices_dir (char **devices_loc)
{
  char *t_devices_include_loc = *devices_loc;
  /* Go up a directory to sysroot.  */
  t_devices_include_loc = msp430_dirname (msp430_dirname (t_devices_include_loc));
  char rest_of_devices_path[] = "/msp430-elf/include/devices/";
  t_devices_include_loc = concat (t_devices_include_loc, rest_of_devices_path, NULL);
  *devices_loc = t_devices_include_loc;
}

int
check_env_var_for_devices (char **local_devices_csv_loc)
{
  const char dirsep[2] = { DIR_SEPARATOR, 0 };
  FILE * devices_csv_file;
  char *collect_gcc = getenv("COLLECT_GCC");
  char *msp430_gcc_include_dir = getenv ("MSP430_GCC_INCLUDE_DIR");
  char *vars_to_try[] = { msp430_gcc_include_dir, collect_gcc };

  int i;
  for (i = 0; i < 2; i++)
    {
      if (vars_to_try[i] == NULL)
	continue;
      char *t_devices_loc = ASTRDUP (vars_to_try[i]);
      devices_csv_file = NULL;
      /* msp430_gcc_include_dir */
      if (i == 0)
	{
	  if (!IS_DIR_SEPARATOR (t_devices_loc[strlen (t_devices_loc) - 1]))
	    t_devices_loc = concat (t_devices_loc, dirsep, NULL);
	  t_devices_loc = concat (t_devices_loc, "devices.csv", NULL);
	}
      /* collect_gcc */
      else if (i == 1)
	{
	  process_collect_gcc_into_devices_dir (&t_devices_loc);
	  t_devices_loc = concat (t_devices_loc, "devices.csv", NULL);
	}
      devices_csv_file = fopen (t_devices_loc,  "r");
      if (devices_csv_file != NULL)
	{
	  fclose (devices_csv_file);
	  *local_devices_csv_loc = t_devices_loc;
	  canonicalize_path_dirsep (local_devices_csv_loc );
	  return 0;
	}
    }
  return 1;
}

void
find_devices_csv (const char * mcu_name)
{
  /* This function only needs to be executed once, but it can be first called
     from a number of different locations.  */
  char *local_devices_csv_loc = NULL;
  static int executed = 0;
  if (executed == 1)
    return;
  executed = 1;
  if (devices_csv_loc)
    parse_devices_csv (devices_csv_loc, mcu_name);
  else if (!check_env_var_for_devices (&local_devices_csv_loc))
    parse_devices_csv (local_devices_csv_loc, mcu_name);
  else if (!TARGET_DISABLE_DEVICE_WARN)
    warning (0, "devices.csv not found on any include paths.\n%s",
	     advice_string);
}

void
parse_devices_csv (const char * real_devices_csv_loc, const char * mcu_name)
{
  FILE * devices_csv = fopen (real_devices_csv_loc, "r");
  /* devices_csv should never be NULL at this stage.  */
  if (devices_csv == NULL)
    {
      if (!TARGET_DISABLE_DEVICE_WARN)
	warning (0, "Unexpected error opening devices.csv\n");
      return;
    }
  /* Some devices have a large number of errata, which means that MPY_TYPE
     isn't found until the ~100th character in the line.  line_buf_siz set to
     200 to be safe and hopefully future proof.  */
  const int line_buf_siz = 200;
  char line[line_buf_siz];
  const char comma[2] = ",";
  char * res;
  int found_headings = 0;
  int cpu_type = -1;
  int mpy_type = -1;
  int cpu_type_column = -1;
  int mpy_type_column = -1;
  const char * device_name_heading = "# Device Name";
  const char * cpu_type_heading = "CPU_TYPE";
  const char * mpy_type_heading = "MPY_TYPE";
  while (1)
    {
      res = fgets (line, line_buf_siz, devices_csv);
      if (res == NULL)
	{
	  if (!TARGET_DISABLE_DEVICE_WARN)
	    warning (0, "Device %s not found in devices.csv.  Check the device "
		     "name is correct or\n%s", mcu_name, advice_string+6);
	  goto end;
	}
      else if (strncmp (line, device_name_heading,
			strlen (device_name_heading)) == 0)
	{
	  found_headings = 1;
	  int curr_column = 0;
	  char * heading = strtok (line, comma);
	  while (heading != NULL)
	    {
	      if (strncmp (heading, cpu_type_heading,
			   strlen (cpu_type_heading)) == 0)
		  cpu_type_column = curr_column;
	      else if (strncmp (heading, mpy_type_heading,
			   strlen (mpy_type_heading)) == 0)
		  mpy_type_column = curr_column;
	      heading = strtok (NULL, comma);
	      curr_column++;
	    }
	  if (curr_column < cpu_type_column || curr_column < mpy_type_column)
	    {
	      if (!TARGET_DISABLE_DEVICE_WARN)
		warning (0, "Couldn't read the required data from devices.csv "
			 "into a buffer.  There may be too many CPU_Bugs in "
			 "the row for %s.  Try removing these so CPU_TYPE and "
			 "MPY_TYPE fit in the buffer.", target_mcu);
	      goto end;
	    }
	  else if (cpu_type_column == -1 || mpy_type_column == -1)
	    {
	      if (!TARGET_DISABLE_DEVICE_WARN)
		warning (0, "CPU_TYPE and/or MPY_TYPE headings not present "
			 "in devices.csv, or format not as expected.\n%s",
			 advice_string);
	      goto end;
	    }
	}
      else if (strncasecmp (line, mcu_name, strlen (mcu_name)) == 0)
	{
	  if (found_headings == 0)
	    {
	      if (!TARGET_DISABLE_DEVICE_WARN)
		warning (0, "Column headings format of devices.csv not as "
			 "expected.\n%s", advice_string);
	      goto end;
	    }
	  extracted_mcu_data.name = mcu_name;
	  char * val = strtok (line, comma);
	  int final_col_num = (cpu_type_column < mpy_type_column)
	    ? mpy_type_column : cpu_type_column;
	  int curr_col;
	  for (curr_col = 0; curr_col < final_col_num + 1; curr_col++)
	    {
	      if (curr_col == cpu_type_column)
		{
		  cpu_type = atoi (val);
		  if (strlen (val) != 1 || (cpu_type == 0 && val[0] != '0')
		      || cpu_type > 2 || cpu_type < 0)
		    {
		      if (!TARGET_DISABLE_DEVICE_WARN)
			warning (0, "Invalid CPU_TYPE read from devices.csv.\n"
				 "%s", advice_string);
		      goto end;
		    }
		  extracted_mcu_data.revision = cpu_type;
		}
	      else if (curr_col == mpy_type_column)
		{
		  mpy_type = atoi (val);
		  if ((mpy_type == 0 && val[0] != '0')
		      || !(mpy_type == 0
			   || mpy_type == 1
			   || mpy_type == 2
			   || mpy_type == 4
			   || mpy_type == 8))
		    {
		      if (!TARGET_DISABLE_DEVICE_WARN)
			warning (0, "Invalid MPY_TYPE read from devices.csv.\n"
				 "%s", advice_string);
		      goto end;
		    }
		  extracted_mcu_data.hwmpy = mpy_type;
		}
	      val = strtok (NULL, comma);
	    }
	  if (cpu_type == -1 || mpy_type == -1)
	    if (!TARGET_DISABLE_DEVICE_WARN)
	      warning (0, "Unknown error reading CPU_TYPE and/or MPY_TYPE from "
		       "devices.csv.\n%s\n", advice_string);
	  goto end;
	}
    }
 end:
  fclose (devices_csv);
}
