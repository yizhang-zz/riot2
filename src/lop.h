#ifndef LOP_H
#define LOP_H

// logical operator is a kind of binding object
// another one is data objects
#include "xbo.h"

class LOP:public XBO
{
	public:
	int nchild;
	XBO * child[MAX_CHILD];
};
	

#endif
