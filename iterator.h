#ifndef ITERATOR_H
#define ITERATOR_H
#ifndef __cplusplus
#include <stdbool.h>
#endif

/* This is intended to act as an abstract class */
typedef struct generic_iterator {
	void (*first)(struct generic_iterator*);
	void (*next)(struct generic_iterator*);
	void (*last)(struct generic_iterator*);
	bool (*end)(struct generic_iterator*);
	struct generic_iterator *(*destroy_iterator)(struct generic_iterator*);
} generic_iterator;

#endif
