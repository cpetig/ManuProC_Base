#include "postgres.h"
#include "fmgr.h"
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(ean_check);
Datum ean_check(PG_FUNCTION_ARGS)
{  
   int number=0;
   int sum=0,i=1;

   if (PG_ARGISNULL(0)) 
    PG_RETURN_NULL();
   number=PG_GETARG_INT32(0);   
   
   while(number)
    {
      sum+= (number%10) * (i & 1 ? 3 : 1);
      i++;
      number/=10;
    }
   if (!(sum %= 10)) 
     sum = 10;

   PG_RETURN_INT32(10-sum);
}

