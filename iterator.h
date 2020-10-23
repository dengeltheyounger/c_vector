#ifndef ITERATOR_H
#define ITERATOR_H
#include <stdbool.h>

typedef struct generic_iterator {
	void (*first)(struct generic_iterator*);
	void (*next)(struct generic_iterator*);
	void (*last)(struct generic_iterator*);
	bool (*end)(struct generic_iterator*);
} generic_iterator;

#endif
