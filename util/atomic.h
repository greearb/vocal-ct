


#ifndef __VOCAL_ATOMIC_H__
#define __VOCAL_ATOMIC_H__



//TODO:  There has got to be a standard way to do this....

#define atomic_t unsigned long int

#define atomic_add(v, b)  (*(v) += (b))
#define atomic_sub(v, b)  (*(v) -= (b))

#define atomic_inc(v) atomic_add(v, 1)
#define atomic_dec(v) atomic_sub(v, 1)

#define atomic_read(v) (*v)



#endif
