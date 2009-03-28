#ifndef COMMON_H
#define COMMON_H

#define NULL 0
#define MAX_DIM 3
#define MAX_CHILD 3
#define VTHRESHOLD 1000

#define USE_RINTERNALS
#include "R.h"
#include "Rinternals.h"

#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
typedef long long int lsize_t;

//enum XDOType {MDO, PDO};
enum OpType {INDEX, ADD, SUBTRACT, MULTIPLY, DIVIDE, POW, SQRT};

template <class T>
inline T max(T a, T b)
{
	if (a>=b)
		return a;
	return b;
}

void riotVectorFinalizer(SEXP p);
#endif

