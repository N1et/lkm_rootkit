// hook linked list 
// manage syscalls hooks

#include <linux/module.h>
#include <linux/slab.h>

#include "include/hooklib.h"

// create_list
void init_list(void){
   hook_list.syscall = 0;
   hook_list.original_call = NULL;
   hook_list.mod_call = NULL;
   hook_list.next = NULL;
}

// retorna a funcao original

void* ( *sys_original(int syscall) ) (){
   struct sys_hook_t *hook;
   for(hook=&hook_list; hook != NULL ; hook=hook->next)
      if(hook->syscall == syscall)
         return (void*(*)()) hook->original_call;
    return NULL;
}
// retorna o ultimo elemento 
struct sys_hook_t* last_sys_hook(void){
   struct sys_hook_t *hook;
   for(hook=&hook_list; hook->next != NULL ; hook=hook->next);
   return hook;
}
// retorna a estrutura baseada na syscall
struct sys_hook_t* get_sys_hook(int syscall){
   struct sys_hook_t *hook = &hook_list;
   do{
      hook = hook->next;
      if(hook->syscall == syscall)
         return hook;
   } while(hook != NULL);

   return NULL;
}

void sys_call_set(int syscall, void *call){
   //Disable Mem Protection
   preempt_disable();
   write_cr0(read_cr0() & (~ 0x10000));

   sys_call_table[syscall] = call;

   //Enable Mem Protection
   preempt_enable();
   write_cr0(read_cr0() | 0x10000);
}
// remove_sys_hook(__NR_open);
int remove_sys_hook(int syscall){
   struct sys_hook_t *hook_toremove;
   void* original_call;
   hook_toremove = get_sys_hook(syscall);
   if(hook_toremove == NULL)
      return 1;
   original_call = hook_toremove->original_call;
   sys_call_set(syscall, original_call);
   kfree(hook_toremove);
   return 0;
}
// remove all sys hooks
void remove_all_sys_hook(void){
   struct sys_hook_t *hook = &hook_list;
   int syscall;
   hook = hook->next;
   do{
      syscall = hook->syscall;
      hook = hook->next;
      remove_sys_hook(syscall);
   } while(hook != NULL);

}

// create_sys_hook(_NR_open, &our_sys_open)
void create_sys_hook(int syscall, void* mod_call ){
   struct sys_hook_t *last_hook = last_sys_hook();
   struct sys_hook_t *hook = kmalloc(sizeof(struct sys_hook_t), GFP_KERNEL);
   hook->syscall = syscall;
   hook->original_call = sys_call_table[syscall];
   hook->mod_call = mod_call;
   hook->next = NULL;
   last_hook->next = hook;
   sys_call_set(syscall, mod_call);
}


