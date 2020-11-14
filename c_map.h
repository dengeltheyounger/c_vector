#ifndef C_MAP_H
#define C_MAP_H
#ifndef __cplusplus
#include <stdlib.h>
#include <stdbool.h>
#else
#include <cstdlib>
#endif
#include "red_black_tree.h"
#include "iterator.h"
#include "error.h"

/* c_map acts as a high level wrapper for the red and black tree 
 * One advantage to having this wrapper is that different map schemes
 * (ordered, unordered) can be accomodated.
 */

#define define_map(K, V)	\
	define_rbtree(K,V)		\
	typedef struct c_map_##K##_##V {	\
		rb_tree(K,V) *tree;	\
		struct c_map_##K##_##V *(*destroy_map)(struct c_map_##K##_##V*);	\
		error_code (*insert)(struct c_map_##K##_##V*, K, V);	\
		error_code (*delete_pair)(struct c_map_##K##_##V*, K);	\
		V (*get_value)(struct c_map_##K##_##V*, K);	\
		bool (*is_key)(struct c_map_##K##_##V*, K);	\
	} c_map_##K##_##V;	\
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
	error_code insert_map_##K##_##V(c_map(K,V) *map, K key, V value) {	\
		error_code result = map->tree->insert(map->tree, key, value);	\
		if (result != success) {	\
			/* The error struct has already been set. Therefore, just return */	\
			return err;	\
		}	\
		return success;	\
	}	\
		\
	error_code delete_pair_map_##K##_##V(c_map(K,V) *map, K key) {	\
		error_code result = map->tree->delete_pair(map->tree, key);	\
		if (result != success) {	\
			return err;	\
		}	\
		return success;	\
	}	\
		\
	bool is_key_map_##K##_##V(c_map(K,V) *map, K key) {	\
		return map->tree->check_key(map->tree, key);	\
	}	\
		\
	K last_key_map_##K##_##V(c_map(K,V) *map) {	\
		return map->tree->last_key(map->tree);	\
	}	\
		\
	K first_key_map_##K##_##V(c_map(K,V) *map) {	\
		return map->tree->first_key(map->tree);	\
	}	\
		\
	K next_key_map_##K##_##V(c_map(K,V) *map, K key) {	\
		K retkey = map->tree->next_key(map->tree, key);	\
		return retkey;	\
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
		map->is_key = &is_key_map_##K##_##V;	\
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
	define_map_iterator(K,V)	\


#define	define_map_iterator(K,V)	\
typedef struct map_iterator_##K##_##V {	\
	generic_iterator geniter;	\
	K key;	\
	V value;	\
	bool lastkey;	\
	c_map(K,V) *map;	\
} map_iterator_##K##_##V;	\
	\
void first_map_iterator_##K##_##V(generic_iterator *generic) {	\
	map_iterator(K,V) *iter = (map_iterator(K,V) *) generic;	\
	iter->key = first_key_map_##K##_##V(iter->map);	\
	iter->value = iter->map->get_value(iter->map, iter->key);	\
	iter->lastkey = false;	\
}	\
	\
void next_map_iterator_##K##_##V(generic_iterator *generic) {	\
	map_iterator(K,V) *iter = (map_iterator(K,V) *) generic;	\
	K key;	\
	K prevkey;	\
	memset(&key, 0, sizeof(K));	\
	prevkey = iter->key;	\
	iter->key = next_key_map_##K##_##V(iter->map, iter->key);	\
	/* next_key will return 0 in two cases. In the first case, */	\
	/* next_key returns 0 if the provided key was not found */	\
	/* It also returns 0 if it was unable to find a successor. */	\
	/* The latter case is most likely. Eventually a more sophisticated method will be found */	\
	if (iter->key == key) {	\
		iter->key = prevkey;	\
		return; \
	}	\
		\
	iter->value = iter->map->get_value(iter->map, iter->key);	\
}	\
	\
/* Set the iterator key and value to the maximum of the tree */	\
void last_map_iterator_##K##_##V(generic_iterator *generic) {	\
	map_iterator(K,V) *iter = (map_iterator(K,V) *) generic;	\
	iter->key = last_key_map_##K##_##V(iter->map);	\
	iter->value = iter->map->get_value(iter->map, iter->key);	\
}	\
	\
/* Compare the iterator key and value with the maximum of the tree */	\
bool end_map_iterator_##K##_##V(generic_iterator *generic) {	\
	map_iterator(K,V) *iter = (map_iterator(K,V) *) generic;	\
	if (iter->lastkey) {	\
		iter->lastkey = false;	\
		return true;	\
	}	\
	K lastkey = last_key_map_##K##_##V(iter->map);	\
	/* If we got the last key, set lastkey and then return false */	\
	/* this will allow the last key to be counted before terminating */	\
	if (iter->key == lastkey)	\
		iter->lastkey = true;	\
								\
	return false;	\
}	\
	\
static inline void set_map_iterator_ptr_##K##_##V(map_iterator(K,V) *iter) {	\
	generic_iterator *giter = (generic_iterator *) iter;	\
	giter->first = &first_map_iterator_##K##_##V;	\
	giter->next = &next_map_iterator_##K##_##V;	\
	giter->last = &last_map_iterator_##K##_##V;	\
	giter->end = &end_map_iterator_##K##_##V;	\
	giter->destroy_iterator = &destroy_iterator;	\
}	\
	\
generic_iterator *new_map_iterator_##K##_##V(c_map(K,V) *map) {	\
	if (map == NULL)	\
		return NULL;	\
	generic_iterator *mi = (generic_iterator *) calloc(1, sizeof(map_iterator(K,V)));	\
	map_iterator(K,V) *iter = (map_iterator(K,V) *) mi;	\
	if (mi == NULL)	\
		return NULL;	\
	iter->map = map;	\
	iter->key = first_key_map_##K##_##V(iter->map);	\
	iter->value = iter->map->get_value(iter->map, iter->key);	\
	iter->lastkey = false;	\
	set_map_iterator_ptr_##K##_##V(iter);	\
	return mi;	\
}	\
	
#define c_map(K,V)	c_map_##K##_##V
#define	new_c_map(K, V)	new_map_##K##_##V()
#define map_iterator(K,V)	map_iterator_##K##_##V
#define new_map_iterator(K,V, MAP)	new_map_iterator_##K##_##V(MAP)

#endif
