#ifndef C_MAP_H
#define C_MAP_H
#include "c_vector.h"
#include <stdlib.h>

/* The internals of this c_map will be almost entirely replaced. 
 * Instead of using an c_vector of internal maps, a red and black tree
 * will be used to handle key value pairs.
 */

typedef enum map_code {
	vector_allocate_failure
} map_code;

#define define_map(K, V)	\
	typedef struct internal_map_##K##_##V {	\
		K key;	\
		V value;	\
	} internal_map_##K##_##V;	\
						\
	define_vector(internal_map_##K##_##V);	\
									\
	typedef struct c_map_##K##_##V {	\
		c_vector(internal_map_##K##_##V) *_vector;	\
		struct c_map_##K##_##V *(*destroy_map)(struct c_map_##K##_##V*);	\
	} c_map_##K##_##V;	\
						\
						\
	c_map(K,V) *destroy_map_##K##_##V(c_map(K,V) *map) {	\
		if (map == NULL) {	\
			return NULL;	\
		}	\
			\
		if (map->_vector != NULL) {	\
			c_vector(internal_map_##K##_##V) *vector = NULL;	\
			vector = map->_vector;	\
			vector = vector->destroy_vector(vector);	\
		}	\
		free(map);	\
		return NULL;	\
	}	\
		\
	static inline void set_map_ptr_##K##_##V(c_map(K,V) *map) {	\
		map->destroy_map = &destroy_map_##K##_##V;	\
	}	\
		\
	c_map(K,V) *new_map_##K##_##V() {	\
		c_map(K,V) *map = NULL;	\
									\
		map = (c_map(K,V) *) calloc(1, sizeof(c_map(K,V)));	\
															\
		if (map == NULL) {	\
			return NULL;	\
		}					\
							\
		map->_vector = new_c_vector(internal_map_##K##_##V, 0);	\
																\
		if (map->_vector == NULL) {	\
			free(map);	\
			return NULL;	\
		}	\
		set_map_ptr_##K##_##V(map);	\
		return map;	\
	}	\
	
#define c_map(K,V)	c_map_##K##_##V
#define internal_map(K,V)	internal_map_##K##_##V
#define	new_c_map(K, V)	new_map_##K##_##V()

#endif
