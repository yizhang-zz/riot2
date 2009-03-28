
#include "vector.h"

char* globaldir = "/export/home/yizhang/data";
int varid = 0;

int Vector::initInt(lsize_t cap, int* data, lsize_t len)
{
  ndim = 1;
  dim[0] = cap;
  type = INTSXP;
  refcount = 0; // will be incremented during assignment
  sprintf(file, "%s/%d", globaldir, varid++);
  FILE *f = fopen(file, "w");
  fwrite(this, sizeof(Vector), 1, f);
  lsize_t i;
  for(i=0; i<cap/len; i++) {
    fwrite(data, sizeof(int), len, f);
  }
  lsize_t remain = cap - cap/len;
  fwrite(data, sizeof(int), remain, f);
  fclose(f);
  return 0;
}

int Vector::initDouble(lsize_t cap, double* data, lsize_t len)
{
  ndim = 1;
  dim[0] = cap;
  type = REALSXP;
  refcount = 0; // will be incremented during assignment
  sprintf(file, "%s/%d", globaldir, varid++);
  FILE *f = fopen(file, "w");
  fwrite(this, sizeof(Vector), 1, f);
  lsize_t i;
  for(i=0; i<cap/len; i++) {
    fwrite(data, sizeof(double), len, f);
  }
  lsize_t remain = cap - cap/len;
  fwrite(data, sizeof(double), remain, f);
  fclose(f);
  return 0;
}



/*template <typename T>
Vector<T>::Vector(const Vector<T>& v):
  size(v.size), type(v.type)
{
  strncpy(file, v.file, strlen(v.file));
}
*/
/*template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& v)
{
  if (&v != this) {
	size = v.size;
	type = v.type;
	strncpy(file, v.file, strlen(v.file));
  }
  return *this;
  }*/
  
 /*
template <typename T>
int Vector<T>::set(lsize_t* index, lsize_t indexlen, T* data, lsize_t len) {
	int offset = sizeof(PDOInfo);
	FILE *f = fopen(file, "r+");
	int i;
	for(i=0; i<indexlen; i++) {
		lseek(f, offset+index[i]*sizeof(T), SEEK_SET);
		fwrite(data+i%len, sizeof(T), 1, f);
	}
	fclose(f);
	return 0;
}

	
template <class T>
int Vector<T>::fill(T* data, lsize_t len) {
	int offset = sizeof(PDOInfo);
	FILE *f = fopen(file, "r+");
	int i;
	lseek(f, offset, SEEK_SET);
	for(i=0; i<this->size/len; i++) {
		fwrite(data, sizeof(T), len, f);
	}
	int remain = this->size - this->size/len;
	fwrite(data, sizeof(T), remain, f);
	fclose(f);
}
 */

void Vector::getNext(void *ret, int nsizes, lsize_t *sizes, lsize_t *index)
{
  assert(nsizes==1);
  lsize_t len = sizes[0];
  FILE *f = fopen(file, "r+");
  int offset = sizeof(Vector);
  lsize_t start = 0;
  lsize_t stop = 0;

  if (type == INTSXP)
	{
	  int *data = (int*) ret;

	  // hack: index in R starts at 1
	  offset -= sizeof(int);
	  // try to read in consecutive segments: [start, stop]
	  while (start < len) {
		while (stop+1 < len && index[stop+1]-index[stop]==1) stop++;
		// fill [start,stop] 
		fseeko(f, offset+sizeof(int)*index[start], SEEK_SET);
		fread(data+start, sizeof(int), stop-start+1, f);
		start = stop = stop + 1;
	  }
	}
  else if (type == REALSXP)
	{
	  double *data = (double*) ret;

	  // hack: index in R starts at 1
	  offset -= sizeof(double);
	  // try to read in consecutive segments: [start, stop]
	  while (start < len) {
		while (stop+1 < len && index[stop+1]-index[stop]==1) stop++;
		// fill [start,stop] 
		fseeko(f, offset+sizeof(double)*index[start], SEEK_SET);
		fread(data+start, sizeof(double), stop-start+1, f);
		start = stop = stop + 1;
	  }
	}	  
  fclose(f);
}

