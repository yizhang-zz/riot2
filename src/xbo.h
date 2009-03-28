/* 
class definition for Data Binding Object.
DBO is the parent of types that can bind to a symbol in R
*/

#ifndef XBO_H
#define XBO_H

#include "common.h"

class XBO {
 public:
  int ndim;
  lsize_t dim[MAX_DIM];
  SEXPTYPE type;
  int refcount;
 XBO()
    {
		refcount = 0;
    }

  virtual void getNext(void*, int, lsize_t*, lsize_t*){}
};

#endif
