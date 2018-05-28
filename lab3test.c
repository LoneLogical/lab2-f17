#include "types.h"
#include "stat.h"
#include "user.h"

int grow_heap(int m) {
   int* foo = malloc(m * sizeof(int));
   memset(foo, 0, m * sizeof(int));
   return m;
}

int inf_grow_stack(int n)
{
   grow_heap(n); 
   inf_grow_stack(n+1);

   return n;
}

int grow_stack(int depth) {
   printf(1, "\ngrowing stack at recursion depth %d\n", depth);

   if (depth == 0)
      return depth;
   grow_stack(depth - 1);

   printf(1, "\nreturning at depth %d\n", depth);  
   return depth;
}

int main(int argc, char *argv[])
{
   int pid=0;
   pid=fork();
   if(pid==0){ 
      grow_stack(1000); 
      exit();
   }
   wait();
   exit();
}
