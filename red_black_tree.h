#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
// This is used for memcmp
#include <endian.h>
#include <stdint.h>

#define PRINT_COLOR(NODE)	fprintf(stderr, "%s", NODE->color == RED ? "RED" : "BLACK")

/* I noticed that the red and black tree wasn't sorting properly when I
 * tested with large integers. I also noticed that the memcmp was giving
 * strange results as well. I think the issue is that endianness is messing
 * with the results of memcmp.
 * I'd like to compare arbitrary keys without needing to resort to 
 * memcmp, but I don't know of any better way to compare the byte value of
 * keys. Plus, memcmp is pretty darn fast
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
	struct node_##K##_##V *(*is_sentinel)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*minimum)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*maximum)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*successor)(struct node_##K##_##V*);	\
	struct node_##K##_##V *(*predecessor)(struct node_##K##_##V*);	\
} node_##K##_##V;	\
	\
static inline bool *is_sentinel_##K##_##V(node(K,V) *node) {	\
	return (node->lchild == NULL || node->rchild == NULL);	\
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
	\
	return node;	\
}	\
	\
node(K,V) *set_sentinels_##K##_##V(node(K,V) *node, node(K,V) *sentinel) {	\
	if (sentinel == NULL) {	\
		fprintf(stderr, "sentinel is null, creating a new one\n");	\
		sentinel = make_sentinel_##K##_##V(sentinel);	\
		if (sentinel == NULL)	\
			return NULL;	\
	}						\
	node->rchild = sentinel;	\
	node->lchild = sentinel;	\
	return sentinel;	\
}	\
	\
node(K,V) *minimum_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *prev = node;	\
	while (!node->is_sentinel(temp)) {	\
		prev = temp;	\
		temp = temp->lchild;	\
	}	\
	return temp;	\
}	\
node(K,V) *maximum_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *prev = node;	\
	while (!node->is_sentinel(temp)) {	\
		prev = temp;	\
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
		while (p != NULL && temp != p->rchild) {	\
			temp = p;	\
			p = p->parent;	\
		}	\
		return p;	\
	}	\
	else {	\
		return maximum(temp->lchild);	\
	}	\
	return NULL;	\
}	\
static inline void set_node_ptr_##K#_##V(node(K,V) *node) {	\
	node->destroy_node = &destroy_node_##K##_##V;	\
	node->set_sentinels = &set_sentinels_##K#_##V;	\
	node->is_sentinel = &is_sentinel_##K##_##V;	\
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
	struct rb_tree_##K##_##V *(*destroy_rbtree)(struct rb_tree_##K##_##V*);	\
	rbtree_code (*insert)(struct rb_tree_##K##_##V *, K, V);	\
	void (*inorder_traverse)(struct rb_tree_##K##_##V *, node(K,V) *);	\
	V get_value(struct rb_tree_##K##_##V *, K);	\
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
/* For that reason, I've written a function that will not only compare the byte values */	\
/* of the key, but also account for endianness. */	\
static inline int compare_little_endian(const unsigned char *key, const unsigned char *nkey, size_t bytes) {	\
	for (size_t i = bytes - 1; i >= 0; --i) {	\
		if (key[i] > nkey[i])	\
			return 1;	\
		else if (key[i] < nkey[i])	\
			return -1;	\
	}	\
	return 0;	\
}	\
static inline int compare_big_endian(const unsigned char *key, const unsigned char *nkey, size_t bytes) {	\
	for (size_t i = 0; i < bytes; ++i) {	\
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
	int result = 0;	\
	while (temp != tree->sentinel) {	\
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
/* 0 indicates that there weren't any errors. holder is null if no repair is needed */	\
rbtree_code basic_delete_##K##_##V(rb_tree(K,V) *tree, K key, node(K,V) *holder) {	\
	holder = NULL;	\
	node(K,V) *node = basic_search_##K##_##V(tree, key);	\
	parent(K,V) *parent = node->parent;	\
	/* If the node has two leaves, simply delete the node */	\
	if (node->lchild == tree->sentinel && node->rchild == tree->sentinel) {	\
		(node == parent->lchild) ? (parent->lchild = tree->sentinel) : (parent->rchild = tree->sentinel);	\
		delete node;	\
		return 0;	\
	}	\
	return 0;	\
}	\
V get_value_##K##_##V(rb_tree(K,V) *tree, K key) {	\
	node(K,V) *temp = basic_search_##K##_##V(tree, key);	\
	return (temp != NULL) ? temp->value; 0;	\
}	\
static inline node(K,V) *basic_insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	node(K,V) *node = tree->root;	\
	node(K,V) *temp = tree->root;	\
	K nkey;	\
	int result = 0;	\
	bool end = false;	\
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
static inline node(K,V) *parent(node(K,V) *node) {	\
	return node == NULL ? NULL : node->parent;	\
}	\
static inline node(K,V) *grandparent(node(K,V) *node) {	\
	node(K,V) *p = parent(node);	\
	return parent(p);	\
}	\
static inline node(K,V) *sibling(node(K,V) *node) {	\
	node(K,V) *p = parent(node);	\
	if (p == NULL)	\
		return NULL;	\
	return node == p->lchild ? p->rchild : p->lchild;	\
}	\
static inline node(K,V) *uncle(node(K,V) *node) {	\
	node(K,V) *p = parent(node);	\
	node(K,V) *s = sibling(p);	\
	return s;	\
}	\
static inline color_t uncle_color(node(K,V) *node) {	\
	node(K,V) *u = uncle(node);	\
	return (u->color == RED) ? RED : BLACK;	\
}	\
static inline void recolor_##K##_##V(node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *p = parent(temp);	\
	node(K,V) *u = uncle(temp);	\
	node(K,V) *g = grandparent(temp);	\
	p->color = BLACK;	\
	u->color = BLACK;	\
	g->color = RED;	\
}	\
	\
static inline void rotate_left_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	node(K,V) *pivot = temp->rchild;	\
	node(K,V) *p = parent(temp);	\
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
	node(K,V) *p = parent(temp);	\
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
	node(K,V) *p = parent(temp);	\
	node(K,V) *g = grandparent(temp);	\
	if (temp == p->rchild && p == g->lchild) {	\
		fprintf(stderr, "Performing left rotate.\n");	\
		rotate_left_##K##_##V(tree, p);	\
		temp = temp->lchild;	\
	}	\
	else if (temp == p->lchild && p == g->rchild) {	\
		fprintf(stderr, "Performing right rotate.\n");	\
		rotate_right_##K##_##V(tree, p);	\
		temp = temp->rchild;	\
	}	\
	p = parent(temp);	\
	g = grandparent(temp);	\
	if (temp == p->lchild) {	\
		fprintf(stderr, "node is p's left child. Performing right rotate\n");	\
		rotate_right_##K##_##V(tree, g);	\
	}	\
	else {	\
		fprintf(stderr, "node is p's right child. Performing left rotate\n");	\
		rotate_left_##K##_##V(tree, g);	\
	}	\
	p->color = BLACK; 	\
	g->color = RED; 	\
}	\
	\
static inline void repair_tree_insert_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	fprintf(stderr, "Entering repair tree\n");	\
	while (true) {	\
		if (temp->parent == NULL) {	\
			fprintf(stderr, "temp's parent was null. Therefore, root node was returned.\n");	\
			temp->color = BLACK;	\
			return;	\
		}	\
		else if (temp->parent->color == BLACK) {	\
			fprintf(stderr, "Temp's parent already black. Exiting\n");	\
			return;	\
		}	\
		else if (uncle(temp) != NULL && uncle_color(temp) == RED) {	\
			fprintf(stderr, "Uncle is not null and uncle's color is red. recolor and re-enter\n");	\
			recolor_##K##_##V(temp);	\
			temp = grandparent(temp);	\
		}	\
		else if (uncle(temp) != NULL && uncle_color(temp) == BLACK) {	\
			fprintf(stderr, "Uncle is not null and uncle's color is black. rotate\n");	\
			rotate_##K##_##V(tree, temp);	\
			return;	\
		}	\
	}	\
}	\
	\
rbtree_code insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	node(K,V) *temp = basic_insert_##K##_##V(tree, key, value);	\
	fprintf(stderr, "Color of newly created node: ");	\
	PRINT_COLOR(temp); fprintf(stderr, "\n");	\
	if (temp == NULL)	\
		return basic_insert_failed;	\
	repair_tree_insert_##K##_##V(tree, temp);	\
	fprintf(stderr, "Color of newly created node after repair: ");	\
	PRINT_COLOR(temp); fprintf(stderr, "\n"); 	\
	return 0;	\
}	\
	\
rbtree_code delete_##K##_##V(rb_tree(K,V) *tree, K key) {	\
	node(K,V) *temp = search_##K##_##V(tree, key);	\
	if (temp == NULL)	\
		return key_not_found;
	repair_tree_delete_##K##_##V(tree, temp);	\
	return 0;	\
}	\
void inorder_traverse_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node)	{	\
	/* Only the sentinel has function pointers set to null */	\
	if (node->set_sentinels == NULL && node->destroy_node == NULL)	\
		return;	\
	inorder_traverse_##K##_##V(tree, node->lchild);	\
	if (node->parent == NULL)	\
		fprintf(stderr, "Root node\n");	\
	else	\
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
