#ifndef C_MAP_H
#define C_MAP_H
#include "red_black_tree.h"
//#include <stdlib.h>

// Eventually all codes will be consolidated under a single error_code enum
typedef enum map_code {
	insert_failure = 1,
	invalid_key
} map_code;

/* c_map acts as a high level wrapper for the red and black tree */

#define define_map(K, V)	\
	define_rbtree(K,V)		\
	typedef struct c_map_##K##_##V {	\
		rb_tree(K,V) *tree;	\
		struct c_map_##K##_##V *(*destroy_map)(struct c_map_##K##_##V*);	\
		map_code (*insert)(struct c_map_##K##_##V*, K, V);	\
		map_code (*delete_pair)(struct c_map_##K##_##V*, K);	\
		V (*get_value)(struct c_map_##K##_##V*, K);	\
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
	map_code insert_map_##K##_##V(c_map(K,V) *map, K key, V value) {	\
		rbtree_code result = map->tree->insert(map->tree, key, value);	\
		if (result != 0)	\
			return insert_failure;	\
		return 0;	\
	}	\
		\
	map_code delete_pair_map_##K##_##V(c_map(K,V) *map, K key) {	\
		rbtree_code result = map->tree->delete_pair(map->tree, key);	\
		if (result != 0)	\
			return invalid_key;	\
		return 0;	\
	}	\
		\
	V get_value_map_##K##_##V(c_map(K,V) *map, K key) {	\
		return map->tree->get_value(map->tree, key);	\
	}	\
		\
	static inline void set_map_ptr_##K##_##V(c_map(K,V) *map) {	\
		map->destroy_map = &destroy_map_##K##_##V;	\
		map->insert = &insert_map_##K##_##V;	\
		map->delete_pair = &delete_pair_map_##K##_##V;	\
		map->get_value = &get_value_map_##K##_##V;	\
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
