#include "types.h"
#include "stat.h"
#include "user.h"

int test(int n)
{ 
   //int x = n + 1;
   test(n+1);
   return n;
}

int recurse(int n)
{
	if (n < 2) {
		return 2;
	}
	return recurse(n-1) + recurse(n-2);
}

int main(int argc, char *argv[])
{
   int pid=0;
   pid=fork();
   if(pid==0){
//	   recurse(8000);
       test(1);
       exit();
   }
   wait();
   exit();
}
