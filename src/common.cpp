#include "common.h"

#include "vector.h"
#include "xbo.h"
void deleteXBO(XBO *p)
{
}

void riotVectorFinalizer(SEXP p)
{
  Rprintf("finalizer called\n");
  Vector *v = (Vector*) R_ExternalPtrAddr(p);
  if (v->refcount == 1)
    deleteXBO(v);
  else
    v->refcount--;
}

