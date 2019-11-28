

#ifndef __HOOKLIB
 #define __HOOKLIB


static void** sys_call_table = (void *) 0xffffffff81309260;

struct sys_hook_t {
   int syscall;
   void* original_call;
   void* mod_call;
   struct sys_hook_t *next;
};
static struct sys_hook_t hook_list;

void init_list(void);
void* ( *sys_original(int syscall) ) ();
struct sys_hook_t* last_sys_hook(void);
struct sys_hook_t* get_sys_hook(int syscall);
void sys_call_set(int syscall, void *call);
int remove_sys_hook(int syscall);
void remove_all_sys_hook(void);
void create_sys_hook(int syscall, void* mod_call );
#endif

