#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));

  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id     = 0;
    shm_table.shm_pages[i].frame  = 0;
    shm_table.shm_pages[i].refcnt = 0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {
  struct proc *curproc = myproc();
  int i;
  char *pg_frame = 0;

  acquire(&(shm_table.lock));
  for(i = 0; i < 64; i++) {
    if (shm_table.shm_pages[i].id == id) {
      pg_frame = shm_table.shm_pages[i].frame;
      break;
    }
  }

  //already exists  
  if(pg_frame) { 
    //mappages
    curproc->sz = PGROUNDUP(curproc->sz);

    if (mappages(curproc->pgdir, (char*)curproc->sz, PGSIZE, V2P(pg_frame), PTE_W|PTE_U) == 0) {
      shm_table.shm_pages[i].refcnt = shm_table.shm_pages[i].refcnt + 1;
      *pointer                      = (char*)curproc->sz; //return the pointer to the new virtual address
      curproc->sz                  += PGSIZE;
    } 

    else {
      release(&(shm_table.lock));
      return 0;
    }
  } 

  //find empty page in table and allocate new page 
  else {
    for(i = 0; i < 64; i++) {
      if (shm_table.shm_pages[i].refcnt == 0) {
        char* new_pg = kalloc();
        memset(new_pg, 0, PGSIZE);
 
        curproc->sz  = PGROUNDUP(curproc->sz);

        if (mappages(curproc->pgdir, (void*)curproc->sz, PGSIZE, V2P(new_pg), PTE_W|PTE_U) == 0) {
          shm_table.shm_pages[i].id     = id;  
          shm_table.shm_pages[i].frame  = (char*)new_pg;      
          shm_table.shm_pages[i].refcnt = 1; 

          //'return' the VA of the new page
          *pointer     = (char*)curproc->sz;    
          curproc->sz += PGSIZE;
          break; 
        } 
       
        else {
          release(&(shm_table.lock));
          return 0; 
        }             
      }    
    }    
  }

  release(&(shm_table.lock));

  return 1; 
}

int shm_close(int id) {
  int i;
  acquire(&(shm_table.lock));
  for(i = 0; i < 64; i++) {
    if(shm_table.shm_pages[i].id != id) {
      continue;
    }
   
    struct proc *curproc = myproc();

    //clear process's pgdir entry
    unsigned pgdir_idx = PTX(shm_table.shm_pages[i].frame);
    curproc->pgdir[pgdir_idx] = 0; 

    //this table entry matches the id
    shm_table.shm_pages[i].refcnt = shm_table.shm_pages[i].refcnt - 1;
    if ( !(shm_table.shm_pages[i].refcnt) ) {
      shm_table.shm_pages[i].id = 0;
      kfree(shm_table.shm_pages[i].frame); 
      shm_table.shm_pages[i].frame = 0;
    }
    //can return correctly
    release(&(shm_table.lock));
    return 0;
  }
  //could not find that id
  
  release(&(shm_table.lock));
  return -1; //error
}
