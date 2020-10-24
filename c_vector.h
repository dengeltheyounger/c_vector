// All of this is to c_vector c++ compatible
#ifndef C_VECTOR_H
#define C_VECTOR_H
#ifndef	__cplusplus
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#else
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#endif
#include "iterator.h"

// This can be used to get type information for a c_vector
#define type_name(DATA_TYPE)	#DATA_TYPE
/* 
 * The name has been changed from array to c_vector
 * The following method allows the added advantage of compile-time typechecking
 * and also allows for class-like function pointers
 */
 
 typedef enum array_code {
	 // This allows 0 as a success code
	 success,
	 realloc_failed,
	 memset_failed,
	 invalid_index
} array_code; 

/* code_to_string(CODE)
 * INPUT: CODE -> array_code value
 * OUTPUT: stringified array_code token
 * USAGE: char *error_value = code_to_string(error_code)
 * NOTES: This can be useful if the user desires to implement a logger for this array
 */ 

#define code_to_string(CODE) ({ #CODE; })
 
/* define_vector(DATA)
 * INPUT: DATA -> data type for desired vector
 * OUTPUT: None
 * USAGE: define_vector(int)
 * NOTES: This must be called before the vector and associated functions are used. 
 * define_vector essentially defines a c_vector struct and a set of functions
 * for the given data type. Using the overall approach allows the generality and ease
 * of programming that comes with macros, along with the type safety and space efficiency
 * of functions (since you don't have the same code copied and pasted into main 100 times).
 * Another option would be to use void * and then have macro wrappers that deal with the
 * data types. However, I think that that would be more error prone and more difficult to 
 * program.
 * 
 * c_vector_##DATA *destroy_c_vector_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA* -> c_vector struct pointer of a given data type
 * OUTPUT: None
 * USAGE: vector = vector->destroy_c_vector(vector);
 * NOTES: Destructor for the c_vector
 * 
 * array_code add_top_##DATA(c_vector_##DATA *vector, DATA value)
 * INPUT: c_vector_##DATA* -> c_vector struct pointer, DATA -> value of given data type
 * OUTPUT: enum array_code to indicate error or 0 to indicate success
 * USAGE: array_code code = vector->add_top(vector, value);
 * NOTES: This function is meant to mirror the vector's push_back method. If
 * new memory needs to be allocated, this function will zero the newly allocated
 * memory.
 * 
 * void remove_top_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: none
 * USAGE: vector->remove_top(vector);
 * NOTES: This function uses memset to zero the given index
 * This is important because the data type may not be primitive
 *
 * size_t get_current_index_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: value of c_vector's curr_index member
 * USAGE: size_t value = vector->get_current_index(vector);
 * NOTES: The current index is the next index available for the add_top function.
 * 
 * size_t get_current_size_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: current size of *data member
 * USAGE: size_t value = vector->get_current_size(vector);
 * NOTES: 
 *
 * size_t get_max_size_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: max size of *data member
 * USAGE: size_t value = vector->get_max_size(vector);
 * NOTES: 
 * 
 * array_code insert_##DATA(c_vector_##DATA *vector, size_t index, DATA value)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * size_t index -> index at which to insert value, DATA value -> value to insert
 * OUTPUT: array_code for error or 0 for success
 * USAGE: array_code code = vector->insert(vector, index, value);
 * NOTES: 
 * 
 * DATA value_at_##DATA(c_vector_##DATA *vector, size_t index)
 * INPUT: c_vector_##DATA -> c_vector struct pointer, size_t index -> index at which to insert
 * OUTPUT: value at index on success or 0 if index is out of bounds
 * USAGE: DATA value = vector->value_at(vector, index);
 * NOTES: This will return the value at curr_index if the index is out of bounds
 * 
 * array_code resize_##DATA(c_vector_##DATA *vector, size_t elementnum)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * size_t elementnum -> number of elements that array should now have
 * OUTPUT: 0 for success or array_code for error
 * USAGE: array_code code = vector->resize(vector, elementnum);
 * NOTES: elementnum should be the number of elements, not the number of bytes
 * 
 * array_code shrink_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: 0 for success or array_code for error
 * USAGE: array_code code = vector->shrink(vector);
 * NOTES: This will make max size equal to current size.
 * If current size is already 0, it will free the memory.
 * If the max size is equal to the current size, it will return 0
 *
 * static inline void set_vector_ptr_##DATA(c_vector_##DATA *vector)
 * INPUT: c_vector_##DATA -> c_vector struct pointer
 * OUTPUT: None
 * USAGE: Internal use only
 * NOTES: This sets the function pointers for c_vector struct when new_vector is called
 *  
 * c_vector_##DATA *new_vector_##DATA(size_t number) 
 * INPUT: size_t number -> number of elements to allocate
 * OUTPUT: c_vector struct pointer
 * USAGE: internal use only, use the macro wrapper defined above
 * NOTES: This creates a new vector of a given data type. If the number of elements to
 * allocate is 0, then the function will allocate space for 2 elements. Any memory pre-allocated
 * is zeroed before the struct pointer is returned to the caller
 */
#define define_vector(DATA)	\
	typedef struct c_vector_##DATA { \
		size_t max_size;	\
		size_t current_size; \
		size_t curr_index; \
		DATA *data; \
		const char *data_type; \
		struct c_vector_##DATA *(*destroy_vector)(struct c_vector_##DATA*);	\
		array_code (*add_top)(struct c_vector_##DATA*, DATA value);	\
		array_code (*remove_top)(struct c_vector_##DATA*);	\
		size_t (*get_current_index)(struct c_vector_##DATA*);	\
		size_t (*get_current_size)(struct c_vector_##DATA*);	\
		size_t (*get_max_size)(struct c_vector_##DATA*);	\
		array_code (*insert)(struct c_vector_##DATA*, size_t, DATA);	\
		DATA (*value_at)(struct c_vector_##DATA*, size_t);	\
		array_code (*resize)(struct c_vector_##DATA*, size_t);	\
		array_code (*shrink)(struct c_vector_##DATA*);	\
	} c_vector_##DATA;	\
						\
	c_vector_##DATA *destroy_c_vector_##DATA(c_vector_##DATA* vector) {	\
		if (vector == NULL) {	\
			return NULL;	\
		}	\
			\
		if (vector->data != NULL) {	\
			free(vector->data);	\
		}	\
		free(vector);	\
		return NULL;	\
	}					\
						\
	array_code add_top_##DATA(c_vector_##DATA *vector, DATA value) {	\
		DATA* temp = NULL;	\
		DATA* memtemp = NULL;	\
		if ((vector->curr_index+1) > vector->current_size) {	\
			temp = (DATA*) realloc((void *) vector->data, sizeof(DATA)*2*vector->max_size);	\
				\
			if (temp == NULL) {	\
				return realloc_failed;	\
			}	\
			vector->data = temp;	\
			size_t index = (vector->max_size / sizeof(DATA)) - 1;	\
			memtemp = (DATA*) memset((void *) &(vector->data[index]), 0, sizeof(DATA)*vector->max_size);	\
			if (memtemp == NULL) {	\
				return memset_failed;	\
			}	\
			temp = NULL;	\
			vector->data[vector->curr_index] = value;	\
			vector->current_size = vector->max_size;	\
			vector->max_size *= 2;	\
			++(vector->curr_index);	\
			return success;	\
		}	\
			\
		vector->data[vector->curr_index] = value;	\
		++vector->curr_index;	\
		return success;	\
	}	\
		\
	array_code remove_top_##DATA(c_vector_##DATA *vector) {	\
		DATA *temp = NULL;	\
		if (vector->curr_index == 0) {	\
			return success;	\
		}	\
		size_t index = vector->curr_index - 1;	\
		temp = (DATA *) memset((void *) &vector->data[index], 0, sizeof(DATA));	\
			\
		if (temp == NULL) {	\
			return memset_failed;	\
		}	\
			\
		--(vector->curr_index);	\
		return success;	\
	}	\
	size_t get_current_index_##DATA(c_vector_##DATA *vector) {	\
		return vector->curr_index;	\
	}	\
		\
	size_t get_current_size_##DATA(c_vector_##DATA *vector) {	\
		return (vector->current_size / sizeof(DATA));	\
	}	\
		\
	size_t get_max_size_##DATA(c_vector_##DATA *vector) {	\
		return (vector->max_size / sizeof(DATA));	\
	}	\
		\
		\
	array_code insert_##DATA(c_vector_##DATA *vector, size_t index, DATA value) {	\
		if (index > (vector->current_size - 1)) {	\
			return invalid_index;	\
		}	\
			\
		vector->data[index] = value;	\
			\
		if (index > vector->curr_index) {	\
			vector->curr_index = index;	\
		}	\
			\
		return success;	\
		\
	}	\
		\
	DATA value_at_##DATA(c_vector_##DATA *vector, size_t index) {	\
		if (index >= vector->curr_index) {	\
			return vector->data[vector->curr_index];	\
		}	\
			\
		return vector->data[index];	\
	}	\
		\
	array_code resize_##DATA(c_vector_##DATA *vector, size_t elementnum) {	\
		size_t newsize = elementnum*sizeof(DATA);	\
		DATA *temp = NULL;	\
		DATA *memtemp = NULL;	\
		if (newsize == vector->current_size) {	\
			return success;	\
		}	\
		else if (newsize > vector->current_size && newsize < vector->max_size) {	\
			vector->current_size = newsize;	\
			return success;	\
		}	\
			\
		else if (newsize < vector->current_size) {	\
			size_t index = elementnum - 1;	\
			temp = (DATA*) memset((void *) &(vector->data[index]), 0, (vector->max_size - newsize));	\
			if (temp == NULL) {	\
				return memset_failed;	\
			}	\
			vector->current_size = newsize;	\
			if (vector->curr_index > index) {	\
				vector->curr_index = index;	\
			}	\
				\
			return success;	\
		}	\
			\
		temp = (DATA*) realloc((void *) vector->data, 2*newsize);	\
		if (temp == NULL) {	\
			return realloc_failed;	\
		}	\
		vector->data = temp;	\
		temp = NULL;	\
		size_t index = (vector->max_size / sizeof(DATA)) - 1;	\
		memtemp = (DATA*) memset((void *) &(vector->data[index]), 0, ((2*newsize) - vector->max_size));	\
		if (memtemp == NULL) {	\
			return memset_failed;	\
		}	\
		vector->current_size = newsize;	\
		vector->max_size = 2*newsize;	\
		return success;	\
	}	\
		\
	array_code shrink_##DATA(c_vector_##DATA* vector) {	\
		if (vector->max_size == vector->current_size) {	\
			return success;	\
		}	\
			\
		DATA *temp = NULL;	\
		temp = (DATA *) realloc((void *) vector->data, vector->current_size);	\
			\
		if (temp == NULL) {	\
			return realloc_failed;	\
		}	\
			\
		vector->max_size = vector->current_size;	\
		vector->data = temp;	\
		return success;	\
	}	\
		\
	static inline void set_vector_ptr_##DATA(c_vector_##DATA* vector) {	\
		vector->destroy_vector = &destroy_c_vector_##DATA;	\
		vector->add_top = &add_top_##DATA;	\
		vector->get_current_index = &get_current_index_##DATA; \
		vector->get_current_size = &get_current_size_##DATA; \
		vector->get_max_size = &get_max_size_##DATA; \
		vector->remove_top = &remove_top_##DATA;	\
		vector->insert = &insert_##DATA;	\
		vector->value_at = &value_at_##DATA;	\
		vector->resize = &resize_##DATA;	\
		vector->shrink = &shrink_##DATA;	\
	}	\
	\
	c_vector_##DATA *new_vector_##DATA(size_t number) {	\
		c_vector(DATA) *vector = NULL;	\
										\
		vector = (c_vector(DATA) *) calloc(1, sizeof(c_vector(DATA)));	\
				\
		if (vector == NULL) {	\
			return NULL;	\
		}	\
			\
		if (number == 0) {	\
			vector->data = (DATA*) calloc(2, sizeof(DATA));	\
				\
			if (vector->data == NULL) {	\
				free(vector);	\
				return NULL;	\
			}	\
				\
			vector->max_size = 2*sizeof(DATA);	\
			vector->current_size = 0;	\
		}	\
			\
		else {	\
			vector->data = (DATA*) calloc(2*number, sizeof(DATA));	\
				\
			if (vector->data == NULL) {	\
				free(vector);	\
				return NULL;	\
			}	\
				\
			vector->max_size = 2*number;	\
			vector->current_size = number;	\
		}	\
		vector->curr_index = 0;	\
		vector->data_type = type_name(DATA);	\
		set_vector_ptr_##DATA(vector);	\
		return vector;	\
	}	\
	define_vector_iterator(DATA)	\
	
/* c_vector(DATA)
 * INPUT: DATA -> the data type desired for the array
 * OUTPUT: None
 * USAGE: c_vector(int) *data = NULL;
 * NOTES: This is used a wrapper and abstracts away some of the internal naming conventions above.
 * If the user wishes to reference a c_vector of type double, they can refer to it as 
 * c_vector(double) instead of c_vector_double.
 * Make sure to typedef any multi-word data types. The c_vector can only handle single word data types.
 */
#define c_vector(DATA) c_vector_##DATA
/* new_c_vector(DATA, NUMBER)
 * INPUT: DATA -> the data type desired for the array, NUMBER -> the number of elements to preallocate
 * OUTPUT: Pointer to newly created c_vector struct
 * USAGE: c_vector(int) *vector = new_c_vector(int, 0);
 * NOTES: new_vector_##DATA does not make sense as a function pointer for the c_vector struct. new_vector
 * is in charge of creating the vector struct, and would lead to a catch 22. For that reason, this
 * wrapper was created in order to abstract away the naming convention above. Instead of worrying about
 * how exactly how the data type is used to uniquely name the function, one need only specify the 
 * data type and number of elements.
 */
#define new_c_vector(DATA, NUMBER) new_vector_##DATA((size_t) NUMBER)

#define define_vector_iterator(TYPE)	\
typedef struct vector_iterator_##c_vector_##TYPE {	\
	generic_iterator geniter;	\
	const TYPE (*current)(struct vector_iterator_##c_vector_##TYPE *);	\
	const c_vector_##TYPE *vector;	\
	size_t current_index;	\
} vector_iterator_##c_vector_##TYPE;	\
		\
	void first_vector_iterator_##c_vector_##TYPE(generic_iterator *generic) {	\
		vector_iterator(TYPE) *iter = (vector_iterator(TYPE) *) generic;	\
		iter->current_index = 0;	\
	}	\
		\
	void next_vector_iterator_##c_vector_##TYPE(generic_iterator *generic) {	\
		vector_iterator(TYPE) *iter = (vector_iterator(TYPE) *) generic;	\
		++(iter->current_index);	\
	}	\
		\
	void last_vector_iterator_##c_vector_##TYPE(generic_iterator *generic) {	\
		vector_iterator(TYPE) *iter = (vector_iterator(TYPE) *) generic;	\
		iter->current_index = iter->vector->curr_index;	\
	}	\
		\
	bool end_vector_iterator_##c_vector_##TYPE(generic_iterator *generic) {	\
		vector_iterator(TYPE) *iter = (vector_iterator(TYPE) *) generic;	\
		if (iter->current_index >= iter->vector->curr_index) {	\
			iter->current_index = 0;	\
			return true;	\
		}	\
		return false;	\
	}	\
		\
	TYPE current_vector_iterator_##c_vector_##TYPE(vector_iterator(TYPE) *iter) {	\
		return iter->vector->data[iter->current_index];	\
	}	\
		\
	void set_vector_iterator_ptr_##c_vector_##TYPE(vector_iterator(TYPE) *iter) {	\
		generic_iterator *generic = (generic_iterator *) iter;	\
		generic->first = &first_vector_iterator_##c_vector_##TYPE;	\
		generic->next = &next_vector_iterator_##c_vector_##TYPE;	\
		generic->last = &last_vector_iterator_##c_vector_##TYPE;	\
		generic->end = &end_vector_iterator_##c_vector_##TYPE;	\
		generic->destroy_iterator = &destroy_iterator;	\
		iter->current = &current_vector_iterator_##c_vector_##TYPE;	\
	}	\
	/* The constructor is not generalized in order to abstract away the setting */	\
	/* of the functoin pointer. I'd like to find a way around that, though. */	\
	generic_iterator *new_vector_iterator_##c_vector_##TYPE(c_vector_##TYPE *vector) {	\
		/* vector iterator stores pointer to vector and the current */	\
		/* data pointed to */	\
		if (vector == NULL) {	\
			return NULL;	\
		}	\
			\
		generic_iterator *vi = (generic_iterator *) calloc(1, sizeof(vector_iterator(TYPE)));	\
		if (vi == NULL) {	\
			return NULL;	\
		}	\
		vector_iterator(TYPE) *iter = (vector_iterator(TYPE) *) vi;	\
		iter->vector = vector;	\
		set_vector_iterator_ptr_##c_vector_##TYPE(iter);	\
		return vi;	\
	}	\

/* vector_iterator_##c_vector_##TYPE is the same as vector_iterator_##VECTOR */
#define vector_iterator(TYPE)	vector_iterator_##c_vector_##TYPE	
#define new_vector_iterator(TYPE, vector)	new_vector_iterator_##c_vector_##TYPE(vector)
#endif
