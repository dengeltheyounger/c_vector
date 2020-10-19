#ifndef C_MAP_H
#define C_MAP_H
#include "red_black_tree.h"
//#include <stdlib.h>

typedef enum map_code {
	vector_allocate_failure
} map_code;

/* c_map acts as a high level wrapper for the red and black tree */

#define define_map(K, V)	\
	define_rbtree(K,V)		\
	typedef struct c_map_##K##_##V {	\
		rb_tree(K,V) *tree;	\
		struct c_map_##K##_##V *(*destroy_map)(struct c_map_##K##_##V*);	\
	} c_map_##K##_##V;	\
						\
						\
	c_map(K,V) *destroy_map_##K##_##V(c_map(K,V) *map) {	\
		if (map == NULL) {	\
			return NULL;	\
		}	\
			\
		if (map->tree != NULL) {	\
			map->tree = map->tree->destroy_rbtree(map->tree);	\
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
		map->tree = new_rbtree(K,V);	\
																\
		if (map->tree == NULL) {	\
			free(map);	\
			return NULL;	\
		}	\
		set_map_ptr_##K##_##V(map);	\
		return map;	\
	}	\
	
#define c_map(K,V)	c_map_##K##_##V
#define	new_c_map(K, V)	new_map_##K##_##V()

#endif
