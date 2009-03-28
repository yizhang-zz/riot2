#ifndef ARITH_H
#define ARITH_H

#include "common.h"
#include "lop.h"


class AddOp: public LOP
{
	public:
	OpType optype;
	// matrix[1:3,c(2,4)] will translate to getNext(2, [3,2],[1,2,3,2,4])
	// dimension info is accessible from XBO members

	// TODO: return NA for out of bound subscripts
	void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
	{
		switch(type)
			{
			case INTSXP:
				getNext((int*)ret, nsizes, sizes, indexes);
				break;
			case REALSXP:
				getNext((double*)ret, nsizes, sizes, indexes);
				break;
			}
	}

	private:
	template<class T>	
		void getNext(T* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
		{
			// ideally should get in chunks according to the buffer size
			// for now just do it at once

			lsize_t flatlength = 1;
			int i=0, k, l;
			for (; i< nsizes; i++)
				flatlength *= sizes[i];

			lsize_t *indexesarray[2];
			bool alloc = FALSE;
			if (child[0]->ndim == 1 && child[1]->ndim == 1 && child[0]->dim[0]!=child[1]->dim[0])
				{
					// recycle
					k = child[0]->dim[0]>child[1]->dim[0]? 0 : 1;
					l = 1-k;
					lsize_t larger = child[k]->dim[0];
					lsize_t smaller = child[l]->dim[0];
					indexesarray[k] = indexes;
					alloc = TRUE;
					indexesarray[l] = new lsize_t[flatlength];
					for (i=0; i<flatlength; i++)
						{
							indexesarray[l][i] = 1 + (indexes[i]-1) % smaller; 
						}
				}
			else
				{
					indexesarray[0] = indexesarray[1] = indexes;
				}

			T *buf[2];
			buf[0] = new T[flatlength];
			buf[1] = new T[flatlength];
			for (int c=0; c < nchild; c++)
				{
					child[c]->getNext(buf[c], nsizes, sizes, indexesarray[c]);
				}
			T *retbuf = (T*) ret;
			for (i=0; i<flatlength; i++)
				{
					retbuf[i] = buf[0][i] + buf[1][i];
				}
			delete[] buf[0];
			delete[] buf[1];
			if (alloc)
				delete[] indexesarray[l];
		}
};

class SubtractOp: public LOP
{
	public:
	OpType optype;
	// matrix[1:3,c(2,4)] will translate to getNext(2, [3,2],[1,2,3,2,4])
	// dimension info is accessible from XBO members

	// TODO: return NA for out of bound subscripts
	void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
	{
		switch(type)
			{
			case INTSXP:
				getNext((int*)ret, nsizes, sizes, indexes);
				break;
			case REALSXP:
				getNext((double*)ret, nsizes, sizes, indexes);
				break;
			}
	}

	private:
	template<class T>	
		void getNext(T* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
		{
			// ideally should get in chunks according to the buffer size
			// for now just do it at once

			lsize_t flatlength = 1;
			int i=0, k, l;
			for (; i< nsizes; i++)
				flatlength *= sizes[i];

			T *buf[2];
			bool mdo = FALSE;
			T *retbuf = (T*) ret;

			if (((MDO*)child[1]) != NULL)
				{
					mdo = TRUE;
					buf[1] = (T*) DATAPTR(((MDO*)child[1])->data);

					buf[0] = new T[flatlength];

					if (child[0]->ndim == 1 && child[1]->ndim == 1 && child[0]->dim[0]!=child[1]->dim[0])
						{
							child[0]->getNext(buf[0], nsizes, sizes, indexes);
							lsize_t smaller = child[1]->dim[0];
							for (i=0; i< flatlength; i++)
								retbuf[i] = buf[0][i] - buf[1][(indexes[i]-1)%smaller];
						}
					else
						{
							child[0]->getNext(buf[0], nsizes, sizes, indexes);
							lsize_t smaller = child[1]->dim[0];
							for (i=0; i< flatlength; i++)
								retbuf[i] = buf[0][i] - buf[1][(indexes[i]-1)%smaller];							
						}

					delete[] buf[0];
				}
			else
				{
			lsize_t *indexesarray[2];
			bool alloc = FALSE;
			if (child[0]->ndim == 1 && child[1]->ndim == 1 && child[0]->dim[0]!=child[1]->dim[0])
				{
					// recycle
					k = child[0]->dim[0]>child[1]->dim[0]? 0 : 1;
					l = 1-k;
					lsize_t larger = child[k]->dim[0];
					lsize_t smaller = child[l]->dim[0];
					indexesarray[k] = indexes;
					alloc = TRUE;
					indexesarray[l] = new lsize_t[flatlength];
					for (i=0; i<flatlength; i++)
						{
							indexesarray[l][i] = 1 + (indexes[i]-1) % smaller; 
						}
				}
			else
				{
					indexesarray[0] = indexesarray[1] = indexes;
				}

			buf[0] = new T[flatlength];
			buf[1] = new T[flatlength];
			for (int c=0; c < nchild; c++)
				{
					child[c]->getNext(buf[c], nsizes, sizes, indexesarray[c]);
				}
			for (i=0; i<flatlength; i++)
				{
					retbuf[i] = buf[0][i] - buf[1][i];
				}
			delete[] buf[0];
			delete[] buf[1];
			if (alloc)
				delete[] indexesarray[l];
				}
		}
};

/*
class SubtractOp: public LOP
{
	public:
	OpType optype;
	// matrix[1:3,c(2,4)] will translate to getNext(2, [3,2],[1,2,3,2,4])
	// dimension info is accessible from XBO members

	// TODO: return NA for out of bound subscripts
	void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
	{
		// ideally should get in chunks according to the buffer size
		// for now just do it at once
		switch(type)
			{
			case INTSXP:
				lsize_t flatlength = 1;
				int i=0, k, l;
				for (; i< nsizes; i++)
					flatlength *= sizes[i];

				lsize_t *indexesarray[2];
				bool alloc = FALSE;
				if (child[0]->ndim == 1 && child[1]->ndim == 1 && child[0]->dim[0]!=child[1]->dim[0])
					{
						// recycle
						k = child[0]->dim[0]>child[1]->dim[0]? 0 : 1;
						l = 1-k;
						lsize_t larger = child[k]->dim[0];
						lsize_t smaller = child[l]->dim[0];
						indexesarray[k] = indexes;
						indexesarray[l] = new lsize_t[larger];
						for (i=0; i<child[k]->dim[0]; i++)
							{
								indexesarray[l][i] = 1 + (indexes[i]-1) % smaller; 
							}
					}
				else
					{
						indexesarray[0] = indexesarray[1] = indexes;
					}

				int *buf[2];
				buf[0] = new int[flatlength];
				buf[1] = new int[flatlength];
				for (int c=0; c < nchild; c++)
					{
						child[c]->getNext(buf[c], nsizes, sizes, indexesarray[c]);
					}
				int *retbuf = (int*) ret;
				for (i=0; i<flatlength; i++)
					{
						retbuf[i] = buf[0][i] - buf[1][i];
					}
				delete[] buf[0];
				delete[] buf[1];
				if (alloc)
					delete[] indexesarray[l];
				break;
			}
	}
};
*/
class MultiplyOp: public LOP
{
	public:
	OpType optype;
	// matrix[1:3,c(2,4)] will translate to getNext(2, [3,2],[1,2,3,2,4])
	// dimension info is accessible from XBO members

	// TODO: return NA for out of bound subscripts
	void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
	{
		// ideally should get in chunks according to the buffer size
		// for now just do it at once
		switch(type)
			{
			case INTSXP:
				lsize_t flatlength = 1;
				int i=0, k, l;
				for (; i< nsizes; i++)
					flatlength *= sizes[i];

				lsize_t *indexesarray[2];
				bool alloc = FALSE;
				if (child[0]->ndim == 1 && child[1]->ndim == 1 && child[0]->dim[0]!=child[1]->dim[0])
					{
						// recycle
						k = child[0]->dim[0]>child[1]->dim[0]? 0 : 1;
						l = 1-k;
						lsize_t larger = child[k]->dim[0];
						lsize_t smaller = child[l]->dim[0];
						indexesarray[k] = indexes;
						indexesarray[l] = new lsize_t[larger];
						for (i=0; i<child[k]->dim[0]; i++)
							{
								indexesarray[l][i] = 1 + (indexes[i]-1) % smaller; 
							}
					}
				else
					{
						indexesarray[0] = indexesarray[1] = indexes;
					}

				int *buf[2];
				buf[0] = new int[flatlength];
				buf[1] = new int[flatlength];
				for (int c=0; c < nchild; c++)
					{
						child[c]->getNext(buf[c], nsizes, sizes, indexesarray[c]);
					}
				int *retbuf = (int*) ret;
				for (i=0; i<flatlength; i++)
					{
						retbuf[i] = buf[0][i] * buf[1][i];
					}
				delete[] buf[0];
				delete[] buf[1];
				if (alloc)
					delete[] indexesarray[l];
				break;
			}
	}
};

class DivideOp: public LOP
{
	public:
	OpType optype;
	// matrix[1:3,c(2,4)] will translate to getNext(2, [3,2],[1,2,3,2,4])
	// dimension info is accessible from XBO members

	// TODO: return NA for out of bound subscripts
	void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
	{
		// ideally should get in chunks according to the buffer size
		// for now just do it at once
		switch(type)
			{
			case INTSXP:
				lsize_t flatlength = 1;
				int i=0, k, l;
				for (; i< nsizes; i++)
					flatlength *= sizes[i];

				lsize_t *indexesarray[2];
				bool alloc = FALSE;
				if (child[0]->ndim == 1 && child[1]->ndim == 1 && child[0]->dim[0]!=child[1]->dim[0])
					{
						// recycle
						k = child[0]->dim[0]>child[1]->dim[0]? 0 : 1;
						l = 1-k;
						lsize_t larger = child[k]->dim[0];
						lsize_t smaller = child[l]->dim[0];
						indexesarray[k] = indexes;
						indexesarray[l] = new lsize_t[larger];
						for (i=0; i<flatlength; i++)
							{
								indexesarray[l][i] = 1 + (indexes[i]-1) % smaller; 
							}
					}
				else
					{
						indexesarray[0] = indexesarray[1] = indexes;
					}

				int *buf[2];
				buf[0] = new int[flatlength];
				buf[1] = new int[flatlength];
				for (int c=0; c < nchild; c++)
					{
						child[c]->getNext(buf[c], nsizes, sizes, indexesarray[c]);
					}
				int *retbuf = (int*) ret;
				for (i=0; i<flatlength; i++)
					{
						retbuf[i] = buf[0][i] / buf[1][i];
					}
				delete[] buf[0];
				delete[] buf[1];
				if (alloc)
					delete[] indexesarray[l];
				break;
			}
	}
};

class PowOp: public LOP
{
	public:
	OpType optype;
	// matrix[1:3,c(2,4)] will translate to getNext(2, [3,2],[1,2,3,2,4])
	// dimension info is accessible from XBO members

	// TODO: return NA for out of bound subscripts
	void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
	{
		// ideally should get in chunks according to the buffer size
		// for now just do it at once
		switch(type)
			{
			case REALSXP:
				lsize_t flatlength = 1;
				int i=0;// k, l;
				for (; i< nsizes; i++)
					flatlength *= sizes[i];

				lsize_t *indexesarray[2];
				bool alloc = FALSE;
				// assume child[0] has a larger size
				MDO *exponent = (MDO*) child[1];
				SEXP dexponent;
				PROTECT(dexponent = coerceVector(exponent->data, REALSXP));
				lsize_t larger = child[0]->dim[0];
				lsize_t smaller = length(dexponent);
				indexesarray[0] = indexes;
				indexesarray[1] = new lsize_t[flatlength];
				alloc = TRUE;
				for (i=0; i<flatlength; i++)
					{
						indexesarray[1][i] = (indexes[i]-1) % smaller; 
					}

				double *buf[2];
				buf[0] = new double[flatlength];
				//buf[1] = new double[flatlength];
				buf[1] = REAL(dexponent);
				// no pinelining yet
				for (int c=0; c < 1; c++)
					{
						child[c]->getNext(buf[c], nsizes, sizes, indexesarray[c]);
					}
				double *retbuf = (double*) ret;
				for (i=0; i<flatlength; i++)
					{
						retbuf[i] = pow(buf[0][i], buf[1][indexesarray[1][i]]);
					}
				delete[] buf[0];
				UNPROTECT(1);
				if (alloc)
					delete[] indexesarray[1];
				break;
			}
	}
};

class SqrtOp: public LOP
{
	public:
	OpType optype;
	// matrix[1:3,c(2,4)] will translate to getNext(2, [3,2],[1,2,3,2,4])
	// dimension info is accessible from XBO members

	// TODO: return NA for out of bound subscripts
	SqrtOp():optype(SQRT)
	{
	}
	
	void getNext(void* ret, int nsizes, lsize_t *sizes, lsize_t *indexes)
	{
		// ideally should get in chunks according to the buffer size
		// for now just do it at once
		lsize_t flatlength = 1;
		int i=0;// k, l;
		for (i=0; i < nsizes; i++)
			flatlength *= sizes[i];
		switch(type)
			{
			case REALSXP:

				// no pinelining yet
				double *retbuf = (double*) ret;
				child[0]->getNext(retbuf, nsizes, sizes, indexes);
				for (i=0; i<flatlength; i++)
					{
						retbuf[i] = sqrt(retbuf[i]);
					}
				break;

			default:
				error("sqrt on this type is not implemented\n");
			}
	}
};


#endif
