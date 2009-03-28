#ifndef MDO_H
#define MDO_H

#include "common.h"
#include "xdo.h"

class MDO: public XDO
{
	public:
	SEXP data;

	MDO(SEXP s)
	  {
	    data = s;
		type = TYPEOF(s);
		ndim = 1;
		dim[0] = length(s);
	  }
};

#endif
