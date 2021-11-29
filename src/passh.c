#include <config.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/utsname.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include "builtins.h"
#include "shell.h"

#include "bashgetopt.h"
#include "utils.h"

#include "common.h"

#include "log.h"
#include "log.c"

#include "ini.h"
#include "ini.c"

#include "process_group.h"
#include "process_group.c"

#include "uuid4.h"
#include "uuid4.c"

#include "process_iterator.h"
#include "process_iterator.c"

int passh_builtin (list) WORD_LIST *list; {
  printf("passh world\n");
  fflush (stdout);
  return (EXECUTION_SUCCESS);
}

int passh_builtin_load (s) char *s; {
  printf ("passh builtin loaded\n");
  fflush (stdout);
  return (1);
}

void passh_builtin_unload (s) char *s; {
  printf ("passh builtin unloaded\n");
  fflush (stdout);
}

char *passh_doc[] = {
	"Sample builtin.",
	"",
	"this is the long doc for the sample passh builtin",
	(char *)NULL
};

struct builtin passh_struct = {
	"passh",
	passh_builtin,
	BUILTIN_ENABLED,
	passh_doc,
	"passh",
	0
};
