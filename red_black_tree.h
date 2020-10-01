#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef enum color_t { BLACK, RED } color_t;

typedef enum rbtree_code {
	realloc_failed = 1,
	memcpy_failed
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
} node_##K##_##V;	\
	\
node(K,V) *destroy_node_##K##_##V(node(K,V) *node) {	\
	if (node == NULL) \
		return NULL;	\
						\
	if (node->rchild != NULL)	\
		node->rchild = destroy_node_##K##_##V(node->rchild);	\
	\
	if (node->lchild != NULL) \
		node->lchild = destroy_node_##K##_##V(node->lchild);	\
	\
	free(node);	\
	return NULL;	\
}	\
	\
node(K,V) *new_node_##K##_##V() {	\
	node(K,V) *node = NULL;	\
	\
	node = (node(K,V)*) malloc(sizeof(node(K,V)));	\
	if (node == NULL) {	\
		return NULL;	\
	}	\
		\
	node->rchild = (node(K,V)*) malloc(sizeof(node(K,V)));	\
	\
	if (node->rchild == NULL) {	\
		free(node);	\
		return NULL;	\
	}	\
		\
	node->lchild = (node(K,V)*) malloc(sizeof(node(K,V)));	\
	\
	if (node->lchild == NULL) {	\
		free(node->rchild);	\
		free(node);	\
		return NULL;	\
	}	\
	node->rchild->color = BLACK;	\
	node->rchild->parent = node;	\
	node->rchild->rchild = NULL;	\
	node->rchild->lchild = NULL;	\
	node->lchild->color = BLACK;	\
	node->lchild->parent = node;	\
	node->lchild->rchild = NULL;	\
	node->lchild->lchild = NULL;	\
	node->parent = NULL;	\
	node->destroy_node = &destroy_node_##K##_##V;	\
	node->rchild->destroy_node = &destroy_node_##K##_##V;	\
	node->lchild->destroy_node = &destroy_node_##K##_##V;	\
	return node;	\
}	\

/* define_rbtree(K,V)
 * INPUT: K -> key data type, V -> value data type
 * OUTPUT: none
 * USAGE: define_rbtree(int, char)
 * NOTES: This is used internally by c_map
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
typedef struct rb_tree_##K##_##V {	\
	node(K,V) *node;	\
	struct rb_tree_##K##_##V *(*destroy_rbtree)(struct rb_tree_##K##_##V*);	\
} rb_tree_##K##_##V;	\
	\
	\
rb_tree(K,V) *destroy_rbtree_##K##_##V(rb_tree(K,V) *tree) {	\
	if (tree != NULL) {	\
		if (tree->node != NULL)	\
			tree->node->destroy_node(tree->node);	\
		free(tree);	\
	}	\
	return NULL;	\
}	\
	\
void set_rbtree_ptr_##K##_##V(rb_tree(K,V) *tree) {	\
	tree->destroy_rbtree = &destroy_rbtree_##K##_##V;	\
}	\
	\
rb_tree(K,V) *new_rbtree_##K##_##V() {	\
	rb_tree(K,V) *tree = NULL;	\
		\
	tree = (rb_tree(K,V)*) malloc(sizeof(rb_tree(K,V)));	\
		\
	if (tree == NULL) {	\
		return NULL;	\
	}	\
	node(K,V) *node = NULL;	\
	node = new_node(K,V);	\
		\
	if (node == NULL) {	\
		free(tree);	\
		return NULL;	\
	}	\
	tree->node = node;	\
	set_rbtree_ptr_##K##_##V(tree);	\
	return tree;	\
}	\


#define node(K,V)	node_##K##_##V
#define rb_tree(K,V)	rb_tree_##K##_##V
#define new_rbtree(K,V)	new_rbtree_##K##_##V()
#define new_node(K,V)	new_node_##K##_##V()
#endif
