#ifndef UPDATE_H
#define UPDATE_H

class UpdateOp: public LOP
{

  // 2nd child is index
  // 3rd child is updated value
  // length of 1nd child must be a multiple of 3rd child
 public:

  void getNext(void* ret, int nsizes, lsize_t* sizes, lsize_t* indexes)
  {
	SEXP t = ((MDO*)child[1])->data;
	SEXP s = ((MDO*)child[2])->data;
	
	child[0]->getNext(ret, nsizes, sizes, indexes);

	double *dv = REAL(s);
	int dvlen = length(s);
	switch(type)
	  {
	  case INTSXP:
		int *buf = (int*) ret;
		// assume 1-D vector for now
		for (int i=0; i<sizes[0]; i++)
		  {
			for (int j=0; j<length(t); j++)
			  switch(TYPEOF(t))
				{
				case INTSXP:
				  int *ia = INTEGER(t);
				  if (indexes[i] == ia[j])
					buf[i] = (int) dv[j%dvlen];
				  break;
				case REALSXP:
				  double *da = REAL(t);
				  if (indexes[i] == da[j])
					buf[i] = (int) dv[j%dvlen];
				  break;
				}
		  }
	  }
  }
};

#endif
