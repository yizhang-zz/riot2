#ifndef INDEX_H
#define INDEX_H

#include "common.h"
#include "mdo.h"
#include "lop.h"

class IndexOp: public LOP
{
 public:
  OpType optype;

 IndexOp()
   :optype(INDEX)
  {
  }

  void getNext(void* ret)
  {
    // child[1..] has the indexes for all dimensions
    
    int i,j,k=0;    
    int nsizes = nchild -1;
    lsize_t *sizes = new lsize_t[nsizes];
    lsize_t sum = 0;
    for (i = 1; i < nchild; i++)
      {
	SEXP t = ((MDO*)child[i])->data;
	sum += length(t);
	sizes[i-1] = length(t);
      }
    lsize_t *indexes = new lsize_t[sum];
    for (i =1; i< nchild; i++)
      {	
	SEXP t = ((MDO*)child[i])->data;
	if (TYPEOF(t)==INTSXP)
	  {
	  int *s = INTEGER(t);
	  for (j=0; j<length(t); j++)
	    indexes[k++] = s[j];
	  }
	else if (TYPEOF(t)==REALSXP)
	  {
	    double *d = REAL(t);
	    for (j=0; j<length(t); j++)
	      indexes[k++] = (lsize_t)d[j];
	  }
      }
	
    child[0]->getNext(ret, nsizes, sizes, indexes);
    delete[] sizes;
    delete[] indexes;
  }

  void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
  {
    // ideally should get in chunks according to the buffer size
    // for now just do it at once

    // first child contains vector
    // second child contains indexes, assume in memory: MDO
    //SEXP selfindexobj = (static_cast<MDO*>(child[1]))->data;
    //int* selfindex = INTEGER(selfindexobj);

    int i,j,k=0;    
    lsize_t dimsum = 0;
    for (i = 0; i < nsizes; i++)
      dimsum += sizes[i];

    lsize_t *indexes1 = new lsize_t[dimsum];
    for (i = 0; i < nsizes; i++)
      {
	//	nsizes1[i] = nsizes[i];
	SEXP t = ((MDO*)child[i+1])->data;
	switch (TYPEOF(t))
	{
	case INTSXP:
			int* selfi = INTEGER(t);
			for (j = 0; j < sizes[i]; j++)
			{
					indexes1[k] = selfi[indexes[k]];
					k++;
			}
			break;
	case REALSXP:
			double *selff = REAL(t);
			for (j = 0; j < sizes[i]; j++)
			{
					indexes1[k] = (lsize_t)selff[indexes[k]];
					k++;
			}
			break;	
	}
      }

    child[0]->getNext(ret, nsizes, sizes, indexes1);
    delete[] indexes1;

    /*
    switch(type)
    {
    case INTSXP:
      lsize_t flatlength = 1;
      int i=0;
      for (; i< nsizes; i++)
	flatlength *= sizes[i];
      int *buf[2];
      int *buf[0] = new int[flatlength];
      int *buf[1] = new int[flatlength];
      for (int c=0; c < nchild; c++)
	{
	  child[c]->getNext(buf[c], nsizes, sizes, indexes);
	}
      int *retbuf = (int*) ret;
      for (i=0; i<flatlength; i++)
	retbuf[i] = buf[0][i] + buf[1][i];

      delete[] buf[0];
      delete[] buf[1];
      break;
    }
    */
  }
};

#endif
