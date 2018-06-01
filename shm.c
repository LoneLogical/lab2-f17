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
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
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

  if(pg_frame) { //already exists
    //mappages
	curproc->sz = PGROUNDUP(curproc->sz);
	if ( mappages(curproc->pgdir, curproc->sz, PGSIZE, *pg_frame, PTE_W|PTE_U) ) {
      //mappages worked fine
	  //incr refcnt
	  shm_table.shm_pages[i].refcnt = shm_table.shm_pages[i].refcnt + 1;
	  *pointer = (char *)va; //return the pointer to the new virtual address
	} else {
      //mappages returned an error
	  return -1;
	}
  } else { // does not exist yet
    //find empty entry in shm_table
	for(i = 0; i < 64; i++) {

    }
      
  }


  return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
//you write this too!




return 0; //added to remove compiler warning -- you should decide what to return
}
