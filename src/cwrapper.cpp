

#include "common.h"
#include "vector.h"
#include "lop.h"
#include "mdo.h"
#include "arith.h"
#include "index.h"
#include "update.h"

extern int varid;

lsize_t totalmem = 0;

extern "C" {
void evaluateExpression(XBO *x, void *ret);


		SEXP setMem(SEXP x, SEXP type)
		{
				return R_NilValue;
		}
		
  SEXP getDim(SEXP x)
  {
    SEXP ans;
    XBO* p = (XBO*) R_ExternalPtrAddr(R_do_slot(x,install("ptr")));
    PROTECT(ans = allocVector(INTSXP, p->ndim));
    for (int i=0; i<p->ndim; i++)
      INTEGER(ans)[i] = p->dim[i];
    UNPROTECT(1);
    return ans;
  }

  SEXP subsetAssign(SEXP x, SEXP i, SEXP v)
  {
	// TODO: size of vector could expand after assignment
	SEXP ans, ptr;
	XBO *orig = (XBO*) R_ExternalPtrAddr(R_do_slot(x,install("ptr")));
	LOP *p = new UpdateOp();
	p->type = orig->type;
	p->ndim = 1;
	p->dim[0] = orig->dim[0];
	p->nchild = 3;
	p->child[0] = orig;
	p->child[1] = new MDO(i);
	p->child[2] = new MDO(v);
	p->child[0]->refcount++;
	p->child[1]->refcount++;
	p->child[2]->refcount++;

	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	PROTECT(ptr = R_MakeExternalPtr(p, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
  }
  
SEXP createRiotVector(SEXP x)
{
  SEXP ans, ptr;
  PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
  
  Vector *vec;
  switch(TYPEOF(x))
	{
	case INTSXP:
	  //	  PROTECT(info = allocVector(RAWSXP,sizeof(Vector)));
			vec = new Vector();
			vec->ndim = 1;
			vec->dim[0] = length(x);
	  vec->initInt((lsize_t)length(x), INTEGER(x), (lsize_t)length(x));
	  PROTECT(ptr = R_MakeExternalPtr(vec, R_NilValue, R_NilValue));
	  break;

	case REALSXP:
	  vec = new Vector();
			vec->ndim = 1;
			vec->dim[0] = length(x);
	  vec->initDouble((lsize_t)length(x), REAL(x), (lsize_t)length(x));
	  PROTECT(ptr = R_MakeExternalPtr(vec, R_NilValue, R_NilValue));
	  break;

	default:
	  break;
	}
  R_do_slot_assign(ans, install("ptr"), ptr);
  R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
  UNPROTECT(2);
  return ans;
}


SEXP showRiotVector(SEXP x)
{
	static char* types[26] = {0,0,0,0,0,0,0,0,0,0,"logical",0,0,"integer","real","complex"};
	//Vector *vec = (Vector*)RAW(R_do_slot(x,install("info")));
	SEXP p = R_do_slot(x, install("ptr"));
	XBO *vec = (XBO*) R_ExternalPtrAddr(p);
	Rprintf("Type:\t%s\n", types[vec->type]);
	char buf[512];
	for (int i=0; i<vec->ndim; i++)
		sprintf(buf, "%lld ", vec->dim[i]);
	Rprintf("Dimensions:\t[ %s]\n", buf);
	Rprintf("Shared by:\t%d\n", vec->refcount);
	if (dynamic_cast<PDO*>(vec)!= NULL)
	  Rprintf("Storage:\t%s\n", ((PDO*)vec)->file);
	else
	  Rprintf("Storage:\tIn memory object\n");
	return R_NilValue;
}

SEXP lengthRiotVector(SEXP x)
{
	SEXP p = R_do_slot(x, install("ptr"));
	XBO *vec = (XBO*) R_ExternalPtrAddr(p);
	lsize_t flatlength = 1;
	for (int i=0; i<vec->ndim; i++)
			flatlength *= vec->dim[i];
	return ScalarReal(flatlength);
}

SEXP addRiotVectors(SEXP e1, SEXP e2)
{
	SEXP ans, ptr;
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	//	PROTECT(ptr = R_do_slot(allocVector(RAWSXP,sizeof(LOP)));
	LOP *res = new AddOp();//(LOP*) RAW(info);
	XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(e1, install("ptr")));
	XBO* x2 = (XBO*) R_ExternalPtrAddr(R_do_slot(e2, install("ptr")));
	res->nchild = 2;
	res->child[0] = x1;
	res->child[1] = x2;
	res->ndim = 1;
	res->dim[0] = max(x1->dim[0], x2->dim[0]);
	x1->refcount++;
	x2->refcount++;
	// determine type 
	if (x1->type==REALSXP || x2->type==REALSXP)
	  res->type = REALSXP;
	if (x1->type==INTSXP && x2->type ==INTSXP)
	  res->type = INTSXP;
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
}

SEXP subtractRiotVectors(SEXP e1, SEXP e2)
{
	SEXP ans, ptr;
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	//	PROTECT(ptr = R_do_slot(allocVector(RAWSXP,sizeof(LOP)));
	LOP *res = new SubtractOp();//(LOP*) RAW(info);
	XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(e1, install("ptr")));
	XBO* x2 = (XBO*) R_ExternalPtrAddr(R_do_slot(e2, install("ptr")));
	res->nchild = 2;
	res->child[0] = x1;
	res->child[1] = x2;
	res->ndim = 1;
	res->dim[0] = max(x1->dim[0], x2->dim[0]);
	x1->refcount++;
	x2->refcount++;
	// determine type 
	if (x1->type==REALSXP || x2->type==REALSXP)
	  res->type = REALSXP;
	if (x1->type==INTSXP && x2->type ==INTSXP)
	  res->type = INTSXP;
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
}


		SEXP subtractRiotVectorNumeric(SEXP e1, SEXP e2)
{
	SEXP ans, ptr;
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	//	PROTECT(ptr = R_do_slot(allocVector(RAWSXP,sizeof(LOP)));
	LOP *res = new SubtractOp();//(LOP*) RAW(info);
	XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(e1, install("ptr")));
	res->nchild = 2;
	res->child[0] = x1;
	res->child[1] = new MDO(e2);
	res->ndim = 1;
	res->dim[0] = max(x1->dim[0], res->child[1]->dim[0]);
	x1->refcount++;
	res->child[1]->refcount++;
	// determine type 
	if (x1->type==REALSXP || res->child[1]->type==REALSXP)
	  res->type = REALSXP;
	if (x1->type==INTSXP && res->child[1]->type ==INTSXP)
	  res->type = INTSXP;
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
}

SEXP divideRiotVectors(SEXP e1, SEXP e2)
{
	SEXP ans, ptr;
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	//	PROTECT(ptr = R_do_slot(allocVector(RAWSXP,sizeof(LOP)));
	LOP *res = new DivideOp();//(LOP*) RAW(info);
	XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(e1, install("ptr")));
	XBO* x2 = (XBO*) R_ExternalPtrAddr(R_do_slot(e2, install("ptr")));
	res->nchild = 2;
	res->child[0] = x1;
	res->child[1] = x2;
	res->ndim = 1;
	res->dim[0] = max(x1->dim[0], x2->dim[0]);
	x1->refcount++;
	x2->refcount++;
	// determine type 
	if (x1->type==REALSXP || x2->type==REALSXP)
	  res->type = REALSXP;
	if (x1->type==INTSXP && x2->type ==INTSXP)
	  res->type = INTSXP;
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
}

SEXP multiplyRiotVectors(SEXP e1, SEXP e2)
{
	SEXP ans, ptr;
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	//	PROTECT(ptr = R_do_slot(allocVector(RAWSXP,sizeof(LOP)));
	LOP *res = new MultiplyOp();//(LOP*) RAW(info);
	XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(e1, install("ptr")));
	XBO* x2 = (XBO*) R_ExternalPtrAddr(R_do_slot(e2, install("ptr")));
	res->nchild = 2;
	res->child[0] = x1;
	res->child[1] = x2;
	res->ndim = 1;
	res->dim[0] = max(x1->dim[0], x2->dim[0]);
	x1->refcount++;
	x2->refcount++;
	// determine type 
	if (x1->type==REALSXP || x2->type==REALSXP)
	  res->type = REALSXP;
	if (x1->type==INTSXP && x2->type ==INTSXP)
	  res->type = INTSXP;
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
}

SEXP powRiotVectors(SEXP e1, SEXP e2)
{
	SEXP ans, ptr;
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	LOP *res = new PowOp();//(LOP*) RAW(info);
	XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(e1, install("ptr")));
	//XBO* x2 = (XBO*) R_ExternalPtrAddr(R_do_slot(e2, install("ptr")));
	res->nchild = 2;
	res->child[0] = x1;
	res->child[1] = new MDO(e2);
	res->ndim = 1;
	res->dim[0] = x1->dim[0];
	x1->refcount++;
	res->child[1]->refcount++;
	// determine type 
	res->type = REALSXP;
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
}
		
		SEXP sqrtRiotVector(SEXP e1)
		{
	SEXP ans, ptr;
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	LOP *res = new SqrtOp();//(LOP*) RAW(info);
	XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(e1, install("ptr")));
	res->nchild = 1;
	res->child[0] = x1;
	res->ndim = 1;
	res->dim[0] = x1->dim[0];
	x1->refcount++;

	// determine type 
	res->type = REALSXP;
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
}

  SEXP indexRiotVector(SEXP x, SEXP i)
  {
    SEXP ans, ptr;
    int indexlen = length(i);

    //    PROTECT(info = allocVector(RAWSXP,sizeof(LOP)));
    LOP *res = new IndexOp(); //(LOP*) RAW(info);
    XBO* x1 = (XBO*) R_ExternalPtrAddr(R_do_slot(x, install("ptr")));

    res->nchild = 2;
    res->child[0] = x1;
    res->child[1] = new MDO(i);
    res->ndim = 1;
    res->dim[0] = indexlen;
    x1->refcount++;
    res->child[1]->refcount++;

    // evaluate and return result
    if (length(i)>VTHRESHOLD)
      {
	PROTECT(ans = R_do_new_object(R_getClassDef("riotvector")));
	PROTECT(ptr = R_MakeExternalPtr(res, R_NilValue, R_NilValue));
	R_do_slot_assign(ans, install("ptr"), ptr);
	R_RegisterCFinalizerEx(ptr, riotVectorFinalizer, TRUE);
	UNPROTECT(2);
	return ans;
      }
    else // return a normal R vector
      {
	switch(x1->type)
	  {
	  case INTSXP:
	    PROTECT(ans = allocVector(INTSXP, indexlen));
	    evaluateExpression(res, INTEGER(ans));
	    UNPROTECT(1);
	    return ans;
	    break;
	  case REALSXP:
		PROTECT(ans = allocVector(REALSXP, indexlen));
		evaluateExpression(res, REAL(ans));
		UNPROTECT(1);
		return ans;
	    break;
	  }
      }

    return ans;
  }

  void evaluateExpression(XBO *x, void* res)
  {
    if ((IndexOp*)x != NULL)
      {
	IndexOp *op = (IndexOp*)x;
	op->getNext(res);
      }
  }
} // end of extern C
