#ifndef VECTOR_H
#define VECTOR_H

#include "common.h"
#include "pdo.h"
//char * globaldir = "/export/home/yizhang";
//int varid = 0;

class Vector: public PDO
{
	public:

	static void copy(Vector* target, Vector* src)
	{
		target->ndim = src->ndim;
		memcpy(target->dim, src->dim, sizeof(lsize_t)*src->ndim);
		target->type = src->type;
		strncpy(target->file, src->file, strlen(src->file));
	}

	int initInt(lsize_t, int*, lsize_t);
	int initDouble(lsize_t, double*, lsize_t);

	void getNext(void*, int nsizes, lsize_t *sizes, lsize_t *indexes);

};


//void copyVectorInfo(Vector*, Vector*);
//int initIntVector(Vector*, lsize_t, int*, lsize_t);

	
	//	~Vector();
	//	int set(lsize_t* index, lsize_t indexlen, T* data, lsize_t datalen);
	//	int fill(T* data, lsize_t len);
	//int get(T* data, lsize_t* index, lsize_t len);


#endif
	
