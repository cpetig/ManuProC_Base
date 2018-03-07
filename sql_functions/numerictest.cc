// $Id: artikelkomponente.c,v 1.1 2005/07/01 08:57:16 christof Exp $

extern "C" {
#define using using_C
#define typename typename_C
#define typeid typeid_C
#define namespace namespace_C
#define delete delete_C
#include <postgres.h>
#include <executor/spi.h>	/* this is what you need to work with SPI */
#include <fmgr.h>
#include <utils/numeric.h>
//#include <catalog/pg_type.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif
#ifndef SET_VARSIZE
# define SET_VARSIZE(d,s) (VARATT_SIZEP(d)=s)
#endif

PG_FUNCTION_INFO_V1(numerictest);

#undef namespace
#undef typename
#undef typeid
#undef using
#undef delete
}

typedef int16 NumericDigit;

// I is byte offset (increment by sizeof NumericDigit!)
#define NUMERIC_DIGIT(D,I) (((NumericDigit*)((D)->n_data+(I)))[0])

static float numeric2float(Numeric d)
{
  float res=0;
  float scale=powf(10000.0f, (float)d->n_weight);
  for (unsigned i=0;i<VARSIZE(d)-NUMERIC_HDRSZ;i+=sizeof(NumericDigit),scale*=0.0001f)
  {
    res+=NUMERIC_DIGIT(d,i)*scale;
  }
  return res;
}

//  4294967296
//   100000000  w=2
//       10000  w=1
static unsigned numeric2u32(Numeric d)
{
  if (NUMERIC_SIGN(d) || NUMERIC_IS_NAN(d))
      elog(ERROR, "numeric2u32: argument is NAN or negative");
  // no rounding for now
  if (d->n_weight<0 || VARSIZE(d)<=NUMERIC_HDRSZ) return 0;
  // now at least one digit is valid
  if (d->n_weight>2 || (d->n_weight==2 && NUMERIC_DIGIT(d,0)>=42))
      elog(ERROR, "numeric2u32: argument overflow");
  static const unsigned weights[3]= { 1, 10000, 100000000 };
  int weightindex= d->n_weight;
  unsigned res=0;
  for (unsigned i=0;i<VARSIZE(d)-NUMERIC_HDRSZ && weightindex>=0;i+=sizeof(NumericDigit),--weightindex)
  {
    elog(NOTICE, "%d %d", NUMERIC_DIGIT(d,i), weights[weightindex]);
    res+= NUMERIC_DIGIT(d,i)*weights[weightindex]; 
  }
  return res;
}

static const float tenk=10000.0f;
Numeric float2numeric(float f)
{
   Numeric res;
   if (!f)
   {
      res = (Numeric) palloc(NUMERIC_HDRSZ);
      SET_VARSIZE(res, NUMERIC_HDRSZ);
      res->n_weight = 0;
      res->n_sign_dscale = 0;
   }
   else // assuming 7-8 decimal digits will always fit into 12 digits of any alignment
   {
      res = (Numeric) palloc(NUMERIC_HDRSZ+6);
      SET_VARSIZE(res, NUMERIC_HDRSZ+6);
      res->n_weight = int(floorf(logf(f)/logf(tenk)));
      res->n_sign_dscale = 4*(2-res->n_weight);
      f/= powf(tenk, (float)res->n_weight);
      elog(NOTICE, "we %d sc %d f %f", res->n_weight, res->n_sign_dscale, f);
      for (unsigned i=0; i<3; ++i, f*=tenk)
      {
         ((NumericDigit*)(res->n_data))[i] = NumericDigit(f);
         f-= ((NumericDigit*)(res->n_data))[i];
      }
   }
   return res;
}

extern "C"
Datum numerictest(PG_FUNCTION_ARGS)
{  
   if (PG_ARGISNULL(0)) 
      PG_RETURN_NULL();
   elog(NOTICE, "OID %d", get_fn_expr_argtype(fcinfo->flinfo, 0));
   Numeric d=DatumGetNumeric(PG_GETARG_DATUM(0));

   elog(NOTICE, "arg %ld %d %d", VARSIZE(d)-NUMERIC_HDRSZ, NUMERIC_DSCALE(d), d->n_weight);
   for (unsigned i=0;i<VARSIZE(d)-NUMERIC_HDRSZ;i+=sizeof(NumericDigit))
     elog(NOTICE, "data[%d]=%04d", i, *((NumericDigit*)(d->n_data+i)));
   elog(NOTICE, "to u32: %u", numeric2u32(d));
//   PG_RETURN_FLOAT4(numeric2float(d));
   elog(NOTICE, "value %f", numeric2float(d));
   PG_RETURN_DATUM((Datum)float2numeric(numeric2float(d)));
//   PG_RETURN_NULL();
}

