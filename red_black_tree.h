#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define PRINT_COLOR(NODE)	fprintf(stderr, "%s", NODE->color == RED ? "RED" : "BLACK")

/* rb_tree compares the byte value of keys. This means that the endianness
 * of the machine matters. For that reason, a custom memcmp type function
 * is used that accounts for endianness
 */

static uint32_t endiantest = 0xdeadbeef;
typedef enum endianness { LITTLE, BIG } endianness;
#define ENDIANNESS	({ endianness e = 0;	\
						if (* (const char *) &endiantest == 0xef)	\
							e = LITTLE;	\
						else if (* (const char *) &endiantest == 0xde)	\
							e = BIG;	\
						e;	\
					})

typedef struct error_info {
	char *functname;
	char *line;
	char *code;
} error_info;

// This can be used for error logging
error_info _err = { .functname = 0, .line = 0, .code = 0 };

typedef enum color_t { BLACK, RED } color_t;

typedef enum rbtree_code {
	realloc_failed = 1,
	make_sentinels_failed,
	new_node_failed,
	basic_insert_failed,
	memcpy_failed,
	key_not_found
} rbtree_code;

/* define_node(K,V)
 * INPUT: K -> key data type, V -> value data type
 * OUTPUT: NONE
 * USAGE define_node(int, char)
 * NOTES: This is designed to be used internally by the red and black tree
 * 
 * node(K,V) *destroy_node_##K##_##V(node(K,V) *node)
 * INPUT: node(K,V) *node -> pointer to node based on a given key type and value type
 * OUTPUT: node(K,V) * -> pointer to node based on a given key type and value type
 * USAGE: node = node->destroy_node(node);
 * NOTES: This will recursively delete all child nodes
 * 
 * void set_sentinels_##K##_##V(node(K,V) *node)
 * INPUT: node(K,V) *node -> pointer to node of key and value data types
 * OUTPUT: none
 * USAGE: node->set_sentinels(node);
 * NOTES: This is used internally to initialize sentinel nodes. It will
 * not test to see if the sentinel nodes already exist.
 * 
 * node(K,V) *new_node_##K##_##V()
 * INPUT: none
 * OUTPUT: node pointer of given key type and value type
 * USAGE: node(K,V) *node = new_node(K,V);
 * NOTES: This will create and initialize two sentinal children
 */

/* node_generic contains all of the operations and members that are common to all nodes.
 * node_generic will be a member of node_##K##_##V (not as a pointer). By using indirection,
 * it can appear as though the node_generic members and operations belong to node_##K##_##V.
 * It seems that this could be used to implement a kind of inheritance in C. The real advantage
 * to node_generic is that the functions and struct will only need to be defined once. I think
 * that will make a big difference in terms of executable size. My original method will lead to
 * the same functions being defined repeatedly for each node_##K##_##V defined. 
 * The purpose of kvnodesize is to allow for efficient copying of nodes
 */

typedef struct node_generic {
	color_t color;
	struct node_generic *parent;
	struct node_generic *rchild;
	struct node_generic *lchild;
	bool (*is_sentinel)(struct node_generic*);
	struct node_generic *(*set_sentinels)(struct node_generic*, struct node_generic*);
	struct node_generic *(*sibling)(struct node_generic*);
	struct node_generic *(*uncle)(struct node_generic*);
	struct node_generic *(*grandparent)(struct node_generic*);
	struct node_generic *(*minimum)(struct node_generic*);
	struct node_generic *(*maximum)(struct node_generic*);
	struct node_generic *(*successor)(struct node_generic*);
	struct node_generic *(*predecessor)(struct node_generic*);
	size_t kvnodesize;	// This contains the size of the kv node struct, not the generic node struct
} node_generic;

#define define_node(K,V)	\
typedef struct node_##K##_##V {	\
	color_t color;	\
	K key;	\
	V value;	\
	struct node_##K##_##V *parent;	\
	struct node_##K##_##V *rchild;	\
	struct node_##K##_##V *lchild;	\
	struct node_##K##_##V *(*destroy_node)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*set_sentinels)(struct node_##K##_##V*, struct node_##K##_##V*);	\
	bool (*is_sentinel)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*sibling)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*uncle)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*grandparent)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*minimum)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*maximum)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*successor)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*predecessor)(struct node_##K##_##V*);	\
} node_##K##_##V;	\
	\
static inline bool is_sentinel_##K##_##V(node(K,V) *node) {	\
	bool result = true;	\
	if (node->lchild != NULL && node->rchild != NULL)	\
		result = false;	\
	return result;	\
}	\
node(K,V) *destroy_node_##K##_##V(node(K,V) *node) {	\
	if (node == NULL) \
		return NULL;	\
	/* only a sentinel will have null children. the tree will take care of this */ \
	else if (node->is_sentinel(node))	\
		return NULL;	\
						\
	if (node->rchild != NULL) {	\
		node->rchild = destroy_node_##K##_##V(node->rchild);	\
	}	\
	\
	if (node->lchild != NULL) \
		node->lchild = destroy_node_##K##_##V(node->lchild);	\
	\
	free(node);	\
	return NULL;	\
}	\
	\
static inline node(K,V) *make_sentinel_##K##_##V() {	\
	node(K,V) *node = (node(K,V)*) calloc(1, sizeof(node(K,V)));	\
	node->color = BLACK;	\
	/* this is done so that sentinels are able to check if they are sentinels */	\
	node->is_sentinel = &is_sentinel_##K##_##V;	\
	\
	return node;	\
}	\
	\
node(K,V) *set_sentinels_##K##_##V(node(K,V) *node, node(K,V) *sentinel) {	\
	if (sentinel == NULL) {	\
		sentinel = make_sentinel_##K##_##V(sentinel);	\
		if (sentinel == NULL)	\
			return NULL;	\
	}						\
	node->rchild = sentinel;	\
	node->lchild = sentinel;	\
	return sentinel;	\
}	\
	\
node(K,V) *sibling_##K##_##V(node(K,V) *node) {	\
	if (node == NULL)	\
		return NULL;	\
	node(K,V) *p = node->parent;	\
	if (p == NULL)	\
		return NULL;	\
	return (node == p->lchild) ? p->rchild : p->lchild;	\
}	\
node(K,V) *uncle_##K##_##V(node(K,V) *node) {	\
	node(K,V) *p = node->parent;	\
	return p->sibling(p);	\
}	\
node(K,V) *grandparent_##K##_##V(node(K,V) *node) {	\
	node(K,V) *p = node->parent;	\
	return (p == NULL) ? NULL : p->parent;	\
}	\
node(K,V) *minimum_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	while (!temp->is_sentinel(temp)) {	\
		if (temp->is_sentinel(temp->lchild))	\
			break;	\
		temp = temp->lchild;	\
	}	\
	return temp;	\
}	\
node(K,V) *maximum_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	while (!node->is_sentinel(temp)) {	\
		if (temp->is_sentinel(temp->rchild))	\
			break;	\
		temp = temp->rchild;	\
	}	\
	return temp;	\
}	\
node(K,V) *successor_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *p = node->parent;	\
	if (node->is_sentinel(temp))	\
		return NULL;	\
	else if (node->is_sentinel(temp->rchild)) {	\
		while (p != NULL && temp != p->lchild) {	\
			temp = p;	\
			p = p->parent;	\
		}	\
		return p;	\
	}	\
	else {	\
		return minimum_##K##_##V(temp->rchild);	\
	}	\
	return NULL;	\
}	\
	\
node(K,V) *predecessor_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *parent = node->parent;	\
	if (node->is_sentinel(temp))	\
		return NULL;	\
	else if (node->is_sentinel(temp->lchild)) {	\
		while (parent != NULL && temp != parent->rchild) {	\
			temp = parent;	\
			parent = parent->parent;	\
		}	\
		return parent;	\
	}	\
	else {	\
		return maximum_##K##_##V(temp->lchild);	\
	}	\
	return NULL;	\
}	\
static inline void set_node_ptr_##K##_##V(node(K,V) *node) {	\
	node->destroy_node = &destroy_node_##K##_##V;	\
	node->set_sentinels = &set_sentinels_##K##_##V;	\
	node->is_sentinel = &is_sentinel_##K##_##V;	\
	node->sibling = &sibling_##K##_##V;	\
	node->uncle = &uncle_##K##_##V;	\
	node->grandparent = &grandparent_##K##_##V;	\
	node->minimum = &minimum_##K##_##V;	\
	node->maximum = &maximum_##K##_##V;	\
	node->successor = &successor_##K##_##V;	\
	node->predecessor = &predecessor_##K##_##V;	\
}	\
node(K,V) *new_node_##K##_##V() {	\
	node(K,V) *node = NULL;	\
	\
	node = (node(K,V)*) calloc(1, sizeof(node(K,V)));	\
	if (node == NULL) {	\
		return NULL;	\
	}	\
		\
	node->color = RED;	\
	set_node_ptr_##K##_##V(node);	\
	return node;	\
}	\

/* define_rbtree(K,V)
 * INPUT: K -> key data type, V -> value data type
 * OUTPUT: none
 * USAGE: define_rbtree(int, char)
 * NOTES: This is used internally by c_map
 * 
 * static inline void rotate_left_##K##_##V(node(K,V) *root, node(K,V) *rchild)
 * INPUT: node(K,V) *root -> root node of subtree, node(K,V) *rchild -> right child of subtree
 * OUTPUT: none
 * USAGE: rotate_left_##K##_##V(root, root->rchild)
 * NOTES: This will do a left rotate of tree. rchild becomes root
 * root becomes lchild, and the original lchild of rchild becomes the
 * rchild of root
 * 
 * static inline void rotate_right_##K##_##V(node(K,V) *root, node(K,V) *lchild)
 * INPUT: node(K,V) *root -> root node of subtree, node(K,V) *lchild -> left child of subtree
 * OUTPUT none
 * USAGE: rotate_right_##K##_##V(root, lchild);
 * NOTES: This  will do a right rotate of tree. lchild becomes root
 * root becomes rchild, and the original rchild of lchild becomes the
 * lchild of root
 * 
 * rb_tree(K,V) *destroy_rbtree_##K##_##V(rb_tree(K,V) *tree)
 * INPUT: rb_tree(K,V) *tree -> red black tree struct of given key and value data types
 * OUTPUT: rb_tree(K,V) * -> pointer to red black tree struct
 * USAGE: tree = tree->destroy_rbtree(tree);
 * NOTES: This will free the node. The entire tree will be freed as well
 * 
 * void set_rbtree_ptr_##K##_##V(rb_tree(K,V) *tree)
 * INPUT: rb_tree(K,V) *tree -> pointer to red black tree struct
 * OUTPUT: none
 * USAGE: set_rbtree_ptr_##K##_##V(tree)
 * NOTES: This is used internally by rbtree to set function pointers
 * 
 * rb_tree(K,V) *new_rbtree_##K##_##V()
 * INPUT: none
 * OUTPUT: rb_tree(K,V) * -> pointer to red black tree struct
 * USAGE: rb_tree(int, char) *tree = new_tree(int, char);
 * NOTES: This will create a root node with two sentinel children
 */
 
#define define_rbtree(K,V)	\
define_node(K,V)	\
typedef struct rb_tree_##K##_##V {	\
	node(K,V) *root;	\
	node(K,V) *sentinel;	\
	struct rb_tree_##K##_##V *(*destroy_rbtree)(struct rb_tree_##K##_##V *);	\
	rbtree_code (*insert)(struct rb_tree_##K##_##V *, K, V);	\
	void (*inorder_traverse)(struct rb_tree_##K##_##V *, node(K,V) *);	\
	V (*get_value)(struct rb_tree_##K##_##V *, K);	\
	rbtree_code (*delete_pair)(struct rb_tree_##K##_##V *, K);	\
} rb_tree_##K##_##V;	\
	\
	\
rb_tree(K,V) *destroy_rbtree_##K##_##V(rb_tree(K,V) *tree) {	\
	if (tree != NULL) {	\
		if (tree->root != NULL)	\
			tree->root->destroy_node(tree->root);	\
		if (tree->sentinel != NULL)	\
			free(tree->sentinel);	\
		free(tree);	\
	}	\
	return NULL;	\
}	\
	\
static inline void print_bytes(const void *key, size_t bytes) {	\
	unsigned char *temp = (unsigned char *) key;	\
	for (size_t i = 0; i < bytes; ++i) 	\
		printf("%02x", temp[i]);	\
	\
	printf("\n");	\
}	\
/* memcmp does not account for endianness. This could mess up the insert function */	\
/* This comparison function accounts for endianness. */	\
static inline int compare_little_endian(const unsigned char *key, const unsigned char *nkey, size_t bytes) {	\
	for (int i = bytes - 1; i >= 0; --i) {	\
		if (key[i] > nkey[i]) {	\
			return 1;	\
		}	\
		else if (key[i] < nkey[i])	\
			return -1;	\
	}	\
	return 0;	\
}	\
static inline int compare_big_endian(const unsigned char *key, const unsigned char *nkey, size_t bytes) {	\
	for (int i = 0; i < bytes; ++i) {	\
		if (key[i] >  nkey[i])	\
			return 1;	\
		else if (key[i] <  nkey[i])	\
			return -1;	\
	}	\
	return 0;	\
}	\
static inline int compare_bytes(const void *key, const void *nkey, size_t bytes) {	\
	if (ENDIANNESS == LITTLE)	\
		return compare_little_endian((unsigned char *) key, (unsigned char *)  nkey, bytes);	\
	else	\
		return compare_big_endian((unsigned char *) key, (unsigned char *)  nkey, bytes);	\
}	\
/* This is inline because it is also used by delete */	\
static inline node(K,V) *basic_search_##K##_##V(rb_tree(K,V) *tree, K key) {	\
	node(K,V) *temp = tree->root;	\
	K nkey;	\
	int result = 0;	\
	while (temp != tree->sentinel) {	\
		nkey = temp->key;	\
		result = compare_bytes(&key, &nkey, sizeof(K));	\
		if (result == 0)	\
			return temp;	\
		else if (result < 0)	\
			temp = temp->lchild;	\
		else	\
			temp = temp->rchild;	\
	}	\
	/* Nullity indicates that the key was not found */	\
	return NULL;	\
}	\
	\
V get_value_##K##_##V(rb_tree(K,V) *tree, K key) {	\
	K k;	\
	node(K,V) *temp = basic_search_##K##_##V(tree, key);	\
	if (temp != NULL)	\
		k = temp->value;	\
	else	\
		memset(&k, 0, sizeof(K));	\
	return k;	\
}	\
	\
static inline node(K,V) *basic_insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	node(K,V) *node = tree->root;	\
	node(K,V) *temp = tree->root;	\
	K nkey;	\
	int result = 0;	\
	if (node == NULL) {	\
		temp = new_node(K,V);	\
		if (temp == NULL)	\
			return NULL;	\
		temp->key = key;	\
		temp->value = value;	\
		tree->sentinel = temp->set_sentinels(temp, tree->sentinel);	\
		tree->root = temp;	\
		return tree->root;	\
	}	\
		\
	while (true) {	\
		node = temp;	\
		nkey = node->key;	\
		result = compare_bytes(&key, &nkey, sizeof(K));	\
		if (result == 0) {	\
			node->value = value;	\
			return node;	\
		}	\
		else if (result < 0) {	\
			temp = node->lchild;	\
		}	\
		else {	\
			temp = node->rchild;	\
			\
		}	\
		if (temp == tree->sentinel) {	\
			break;	\
		}	\
	}	\
	temp = new_node(K,V);	\
		\
	if (temp == NULL)	\
		return NULL;	\
	temp->set_sentinels(temp, tree->sentinel);	\
	temp->key = key;	\
	temp->value = value;	\
	temp->color = RED;	\
	temp->parent = node;	\
	(result > 0) ? (node->rchild = temp) : (node->lchild = temp);	\
	return temp;	\
}	\
	\
static inline color_t uncle_color_##K##_##V(node(K,V) *node) {	\
	node(K,V) *u = node->uncle(node);	\
	return (u->color == RED) ? RED : BLACK;	\
}	\
static inline void recolor_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *p = temp->parent;	\
	node(K,V) *u = node->uncle(node);	\
	node(K,V) *g = node->grandparent(node);	\
	p->color = BLACK;	\
	u->color = BLACK;	\
	g->color = RED;	\
}	\
	\
static inline void rotate_left_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *pivot = temp->rchild;	\
	node(K,V) *p = temp->parent;	\
	pivot->parent = p;	\
	temp->rchild = pivot->lchild;	\
	pivot->lchild = temp;	\
	/* Check if temp's rchild is a sentinel */	\
	if (temp->rchild != tree->sentinel) {	\
		temp->rchild->parent = temp;	\
	}	\
	temp->parent = pivot;	\
	if (p != NULL) {	\
		(p->rchild == temp) ? (p->rchild = pivot) : (p->lchild = pivot);	\
	}	\
	/* Check case where node is root */	\
	if (tree->root == temp) {	\
		tree->root = pivot;	\
	}	\
}	\
	\
static inline void rotate_right_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *p = temp->parent;	\
	node(K,V) *pivot = temp->lchild;	\
	pivot->parent = p;	\
	temp->lchild = pivot->rchild;	\
	pivot->rchild = temp;	\
	/* check if temp's lchild is a sentinel */	\
	if (temp->lchild != tree->sentinel) {	\
		temp->lchild->parent = temp;	\
	}	\
	temp->parent = pivot;	\
	/* check case where node is root */	\
	if (p != NULL) {	\
		(p->rchild == temp) ? (p->rchild = pivot) : (p->lchild = pivot);	\
	}	\
	if (tree->root == temp) {	\
		tree->root = pivot;	\
	}	\
}	\
	\
static inline void rotate_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *p = temp->parent;	\
	node(K,V) *g = temp->grandparent(temp);	\
	if (temp == p->rchild && p == g->lchild) {	\
		rotate_left_##K##_##V(tree, p);	\
		temp = temp->lchild;	\
	}	\
	else if (temp == p->lchild && p == g->rchild) {	\
		rotate_right_##K##_##V(tree, p);	\
		temp = temp->rchild;	\
	}	\
	p = temp->parent;	\
	g = temp->grandparent(temp);	\
	if (temp == p->lchild) {	\
		rotate_right_##K##_##V(tree, g);	\
	}	\
	else {	\
		rotate_left_##K##_##V(tree, g);	\
	}	\
	p->color = BLACK; 	\
	g->color = RED; 	\
}	\
	\
static inline void repair_tree_insert_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	while (true) {	\
		/* The only case that doesn't immediately exit is case 3. */	\
		/* In this case, temp is root */	\
		if (temp->parent == NULL) {	\
			temp->color = BLACK;	\
			return;	\
		}	\
		/* In this case, temp is red and parent is black. No need to modify */	\
		else if (temp->parent->color == BLACK) {	\
			return;	\
		}	\
		/* Recolor and then work up the tree doing modifications as necessary */	\
		else if (temp->uncle(temp) != NULL && uncle_color_##K##_##V(temp) == RED) {	\
			recolor_##K##_##V(temp);	\
			temp = temp->grandparent(temp);	\
		}	\
		else if (temp->uncle(temp) != NULL && uncle_color_##K##_##V(temp) == BLACK) {	\
			rotate_##K##_##V(tree, temp);	\
			return;	\
		}	\
	}	\
}	\
	\
rbtree_code insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	/* Insert and then perform tree repairs */	\
	node(K,V) *temp = basic_insert_##K##_##V(tree, key, value);	\
	if (temp == NULL)	\
		return basic_insert_failed;	\
	repair_tree_insert_##K##_##V(tree, temp);	\
	return 0;	\
}	\
	\
/* This function will set the parent of sentinel if v is sentinel */	\
static inline void transplant_##K##_##V(rb_tree(K,V) *tree, node(K,V) *u, node(K,V) *v) {	\
	node(K,V) *parent = u->parent;	\
	if (u->parent == NULL)	\
		tree->root = v;	\
	else	\
		(u == parent->lchild) ? (parent->lchild = v) : (parent->rchild = v);	\
	v->parent = parent;	\
}	\
	\
static inline void repair_tree_delete_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *sibling = NULL;	\
	node(K,V) *parent = NULL;	\
	bool caseone = false;	\
	/* Sibling will be set manually instead of using the function pointer */	\
	/* The reason for this is that the temp may be a sentinel */	\
	while (temp != tree->root && temp->color == BLACK) {	\
		caseone = false;	\
		if (temp == temp->parent->lchild) {	\
			/* It is implicit that the sibling is the right child of parent */	\
			sibling = temp->parent->rchild;	\
			if (sibling->color == RED) {	\
				sibling->color = BLACK;	\
				temp->parent->color = RED;	\
				rotate_left_##K##_##V(tree, temp->parent);	\
				sibling = temp->parent->rchild;	\
				caseone = true;	\
				/* See below mirror case */	\
				if (sibling->is_sentinel(sibling)) {	\
					fprintf(stderr, "Sibling is sentinel. Exiting\n");	\
					break;	\
				}	\
			}	\
			/* case two terminates if it was entered through case one */	\
			if (sibling->lchild->color == BLACK && sibling->rchild->color == BLACK) {	\
				sibling->color = RED;	\
				temp = temp->parent;	\
				if (caseone)	\
					break;	\
			}	\
			else if (sibling->rchild->color == BLACK) {	\
				sibling->lchild->color = BLACK;	\
				sibling->color = RED;	\
				rotate_right_##K##_##V(tree, sibling);	\
				sibling = temp->parent->rchild;	\
			}	\
			/* This is set as a separate if statement because case 3 will lead into */	\
			/* case 4 and case 1 can lead into cases 2,3,4 */	\
			if (sibling->rchild->color == RED)  {	\
				sibling->color = temp->parent->color;	\
				temp->parent->color = BLACK;	\
				sibling->rchild->color = BLACK;	\
				rotate_left_##K##_##V(tree, temp->parent);	\
				/* At the end of case 4, temp is set to root. Just break */	\
				break;	\
			}	\
		}	\
		/* This is literally the exact same except left and right are switched */	\
		else {	\
			sibling = temp->parent->lchild;	\
			if (sibling->color == RED) {	\
				sibling->color = BLACK;	\
				temp->parent->color = RED;	\
				rotate_right_##K##_##V(tree, temp->parent);	\
				sibling = temp->parent->lchild;	\
				caseone = true;	\
				/* In rare cases, the sibling ends up being a sentinel. */ \
				/* It doesn't make sense to continue since sentinels have no */	\
				/* children */	\
				if (sibling->is_sentinel(sibling)) {	\
					fprintf(stderr, "sibling is sentinel. Exiting\n");	\
					break;	\
				}	\
			}	\
			if (sibling->rchild->color == BLACK && sibling->lchild->color == BLACK) {	\
				sibling->color = RED;	\
				temp = temp->parent;	\
				if (caseone)	\
					break;	\
			}	\
			else if (sibling->lchild->color == BLACK) {	\
				sibling->rchild->color = BLACK;	\
				sibling->color = RED;	\
				rotate_left_##K##_##V(tree, sibling);	\
				sibling = temp->parent->lchild;	\
			}	\
			if (sibling->lchild->color == RED) {	\
				sibling->color = temp->parent->color;	\
				temp->parent->color = BLACK;	\
				sibling->lchild->color = BLACK;	\
				rotate_right_##K##_##V(tree, temp->parent);	\
				break;	\
			}	\
		}	\
	}	\
	temp->color = BLACK;	\
}	\
	\
/* This implementation is inspired by pseudocode provided by Cormon's */	\
/* introduction to algorithms, third edition */	\
rbtree_code delete_##K##_##V(rb_tree(K,V) *tree, K key) {	\
	node(K,V) *node = basic_search_##K##_##V(tree, key);	\
	/* mover is either moved out of or within tree */	\
	node(K,V) *mover = node;	\
	/* rmover replaces mover's original position */	\
	node(K,V) *rmover = NULL;	\
	if (node == NULL) {	\
		return key_not_found;	\
	}	\
	/* ocolor keeps track of y's original color */	\
	color_t ocolor = node->color;	\
	/* Replace node with its right child */	\
	if (node->is_sentinel(node->lchild)) {	\
		rmover = node->rchild;	\
		transplant_##K##_##V(tree, node, node->rchild);	\
	}	\
	/* Replace node with its left child */	\
	else if (node->is_sentinel(node->rchild)) {	\
		rmover = node->lchild;	\
		transplant_##K##_##V(tree, node, node->lchild);	\
	}	\
	/* mover points to successor. mover replaces node */	\
	else {	\
		mover = node->successor(node);	\
		ocolor = mover->color;	\
		rmover = mover->rchild;	\
		/* we plan to remove node, so rmover->parent cant be node */	\
		if (mover->parent == node)	\
			rmover->parent = mover;	\
		else {	\
			transplant_##K##_##V(tree, mover, mover->rchild);	\
			mover->rchild = node->rchild;	\
			mover->rchild->parent = mover;	\
		}	\
		transplant_##K##_##V(tree, node, mover);	\
		mover->lchild = node->lchild;	\
		mover->lchild->parent = mover;	\
		mover->color = node->color;	\
	}	\
	/* One or more violations have been introduced if mover was originally black */	\
	if (ocolor == BLACK) {	\
		fprintf(stderr, "Entering deletion repair\n");	\
		repair_tree_delete_##K##_##V(tree, rmover);	\
	}	\
	/* At the end, set the sentinel's parent back to NULL in case its parent was set */	\
	tree->sentinel->parent = NULL;	\
	/* delete node at the end */	\
	free(node);	\
	return 0;	\
}	\
	\
void inorder_traverse_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node)	{	\
	/* No need to print sentinel */	\
	if (node->is_sentinel(node))	\
		return;	\
	inorder_traverse_##K##_##V(tree, node->lchild);	\
	if (node->parent != NULL)	\
		fprintf(stderr, "Node is the child of %d\n", node->parent->key);	\
	fprintf(stderr, "Key: %d, Value: %c\n", node->key, node->value);	\
	fprintf(stderr, "Color: "); PRINT_COLOR(node); fprintf(stderr, "\n");	\
	inorder_traverse_##K##_##V(tree, node->rchild);	\
}	\
	\
void set_rbtree_ptr_##K##_##V(rb_tree(K,V) *tree) {	\
	tree->destroy_rbtree = &destroy_rbtree_##K##_##V;	\
	tree->insert = &insert_##K##_##V;	\
	tree->inorder_traverse = &inorder_traverse_##K##_##V;	\
	tree->get_value = &get_value_##K##_##V;	\
	tree->delete_pair = &delete_##K##_##V;	\
}	\
	\
rb_tree(K,V) *new_rbtree_##K##_##V() {	\
	rb_tree(K,V) *tree = NULL;	\
		\
	tree = (rb_tree(K,V)*) calloc(1, sizeof(rb_tree(K,V)));	\
		\
	if (tree == NULL) {	\
		return NULL;	\
	}	\
	set_rbtree_ptr_##K##_##V(tree);	\
	return tree;	\
}	\


#define node(K,V)	node_##K##_##V
#define rb_tree(K,V)	rb_tree_##K##_##V
#define new_rbtree(K,V)	new_rbtree_##K##_##V()
#define new_node(K,V)	new_node_##K##_##V()
#endif
