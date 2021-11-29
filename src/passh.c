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

#include "Passh.c"

#include "process_group.h"
#include "process_group.c"

#include "uuid4.h"
#include "uuid4.c"

#include "process_iterator.h"
#include "process_iterator.c"


int passh_builtin (list) WORD_LIST *list; {
  printf("passh world\n");
  int argc = list_length(list);
  char **argv;
  START_VLA(char*, argc + 1, argv);
  to_argv(list, argc, (const char**) argv);
  argv[argc] = NULL;
  if (check_no_options(&list) == -1){
   log_error("No Options Supplied");
   return EXECUTION_FAILURE;
  }
  if (argc>0){
    for (int i = 1; list != NULL; list = list->next, ++i){
        if (strcasecmp(list->word->word, "help") == 0){
            log_debug("help.............");
            return EXECUTION_SUCCESS;
        }
        if (strcasecmp(list->word->word, "exec") == 0){
            log_debug("Passh Exec Mode");

            char slave_name[32];
            pid_t pid;
            struct termios orig_termios;
            struct winsize size;

            startup();

            log_debug("%d Args received", argc);

            bool it = isatty(STDIN_FILENO);
            log_debug("tty? %s", (it == 0) ? "true" : "false");


            if(false){
              for (int i = 1; list != NULL; list = list->next, ++i){
                log_debug("argv[%d]: %s", i, list->word->word);
              }
            }
            getargs(argc, argv);


            sig_handle(SIGCHLD, sig_child);

            if (it) {
                if (tcgetattr(STDIN_FILENO, &orig_termios) < 0)
                    log_fatal("tcgetattr error on stdin");
                if (ioctl(STDIN_FILENO, TIOCGWINSZ, (char *) &size) < 0)
                    log_fatal("TIOCGWINSZ error");

                pid = pty_fork(&g.fd_ptym, slave_name, sizeof(slave_name),
                    &orig_termios, &size);

            } else {
                pid = pty_fork(&g.fd_ptym, slave_name, sizeof(slave_name),
                    NULL, NULL);
            }

            if (pid < 0) {
                log_fatal("Passh fork error");
            } else if (pid == 0) {
                log_debug("Passh forked");
                if (g.opt.nohup_child) {
                    sig_handle(SIGHUP, SIG_IGN);
                }
                if (execvp(g.opt.command[0], g.opt.command) < 0)
                    log_fatal("can't execute: %s", g.opt.command[0]);
            }
            log_debug("Passh forked PID %d", pid);

            if (it && isatty(STDOUT_FILENO) ) {
                if (tty_raw(STDIN_FILENO, &g.save_termios) < 0)
                    log_fatal("tty_raw error");

                g.reset_on_exit = true;
                if (atexit(tty_atexit) < 0)
                    log_fatal("atexit error");

                sig_handle(SIGWINCH, sig_winch);
            }
            log_debug("Passh Running Loop");
            big_loop();
            log_debug("Passh Mode End");
            return EXECUTION_SUCCESS;
        }
       log_error("Invalid Mode Supplied- %s", list->word->word);
       return EXECUTION_FAILURE;
    }
  }else{
    log_debug("Passh - %d args\n", argc);
  }
  fflush (stdout);
  return (EXECUTION_SUCCESS);
}

void set_log_level(){
  SHELL_VAR *_log_level = find_variable("LOG_LEVEL");
  if (_log_level != NULL){
    char *log_level = get_variable_value(_log_level);
    if (log_level != NULL && strlen(log_level) > 4){
      if (
        strcasecmp(log_level, "LOG_TRACE") == 0
        || strcasecmp(log_level, "TRACE") == 0
      ){
        log_set_level(LOG_TRACE);
      }else if (
        strcasecmp(log_level, "LOG_DEBUG") == 0
        || strcasecmp(log_level, "DEBUG") == 0
      ){
        log_set_level(LOG_DEBUG);
      }else if (
        strcasecmp(log_level, "LOG_WARN") == 0
        || strcasecmp(log_level, "WARN") == 0
      ){
        log_set_level(LOG_WARN);
      }else if (
        strcasecmp(log_level, "LOG_ERROR") == 0
        || strcasecmp(log_level, "ERROR") == 0
      ){
        log_set_level(LOG_ERROR);
      }else if (
        strcasecmp(log_level, "LOG_FATAL") == 0
        || strcasecmp(log_level, "FATAL") == 0
      ){
        log_set_level(LOG_FATAL);
      }
    }      
  }
}


int passh_builtin_load (s) char *s; {
  fprintf(stderr, "passh builtin loading\n");
  set_log_level();  
  fprintf(stderr, "passh log level- %d (%s)\n", L.level, log_level_string(L.level));
  fprintf(stderr, "passh builtin loaded\n");
  fflush (stdout);
  fflush (stderr);
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
