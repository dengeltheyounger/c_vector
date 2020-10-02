#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

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
	rbtree_code (*set_sentinels)(struct node_##K##_##V*, struct node_##K##_##V*);	\
} node_##K##_##V;	\
	\
node(K,V) *destroy_node_##K##_##V(node(K,V) *node) {	\
	if (node == NULL) \
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
static inline rbtree_code make_sentinel_##K##_##V(node(K,V) *node) {	\
	node = (node(K,V)*) calloc(1, sizeof(node(K,V)));	\
	\
	if (node == NULL) {	\
		return make_sentinels_failed;	\
	}	\
		\
	return 0;	\
}	\
	\
rbtree_code set_sentinels_##K##_##V(node(K,V) *node, node(K,V) *sentinel) {	\
	rbtree_code result;	\
	if (sentinel == NULL) {	\
		result = make_sentinel_##K##_##V(sentinel);
		if (result != 0)	\
			return result;	\
	}						\
	node->rchild = sentinel;	\
	node->lchild = sentinel;	\
}	\
	\
node(K,V) *new_node_##K##_##V() {	\
	rbtree_code result;	\
	node(K,V) *node = NULL;	\
	\
	node = (node(K,V)*) calloc(1, sizeof(node(K,V)));	\
	if (node == NULL) {	\
		return NULL;	\
	}	\
		\
	node->color = RED;	\
	node->destroy_node = &destroy_node_##K##_##V;	\
	node->set_sentinels = &set_sentinels_##K##_##V;	\
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
} rb_tree_##K##_##V;	\
	\
	\
static inline void rotate_left_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *pivot = node->rchild;	\
	node(K,V) *root = node;	\
	root->rchild = pivot->lchild;	\
	pivot->lchild->parent = root;	\
	pivot->lchild = root;	\
	pivot->parent = root->parent;	\
	root->parent = pivot;	\
	root = pivot;	\
	if (root->lchild == tree->root)	\
		tree->root = root;	\
}	\
	\
static inline void rotate_right_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *pivot = node->lchild;	\
	node(K,V) *root = node;	\
	root->lchild = pivot->rchild;	\
	pivot->rchild->parent = root;	\
	pivot->rchild = root;	\
	pivot->parent = root->parent;	\
	root->parent = pivot;	\
	root = pivot;	\
	if (root->rchild == tree->root)
		tree->root = root;	\
}	\
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
static inline node(K,V) *basic_insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	node(K,V) *node = tree->root;	\
	node(K,V) *temp = NULL;	\
	K nkey;	\
	int result = 0;	\
	if (tree->root == NULL) {	\
		temp = new_node(K,V);	\
		if (temp == NULL)	\
			return NULL;	\
		temp->set_sentinels(temp, tree->sentinel);	\
		tree->root = temp;	\
		return tree->root;	\
	}	\
		\
	while (true) {	\
		nkey = node->key;
		result = memcmp(&key, &nkey, sizeof(K));
		if (result == 0) {	\
			node->value = value;	\
			return node;	\
		}	\
		else if (result < 0) {	\
			temp = node->lchild;	\
		}	\
		else {	\
			temp = node->rchild;	\
		}	\
			\
		if (temp == tree->sentinel)	\
			break;	\
	}	\
	temp = new_node(K,V);	\
		\
	if (temp == NULL)	\
		return NULL;	\
	temp->set_sentinels(temp, tree->sentinel);	\
	temp->key = key;	\
	temp->value = value;	\
	temp->parent = node;	\
	(result > 0) ? (node->rchild = temp) : (node->lchild = temp);	\
	return temp;	\
}	\
	\
static inline node(K,V) *parent(node(K,V) *node) {	\
	return node == NULL ? NULL : node->parent;	\
}	\
static inline node(K,V) *grandparent(node(K,V) *node) {	\
	node(K,V) p = parent(node);	\
	return parent(p);	\
}	\
static inline node(K,V) *sibling(node(K,V) *node) {	\
	node(K,V) *p = parent(node);	\
	if (p == NULL)	\
		return NULL;	\
	return node == p->left ? p->right : p->left;	\
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
static inline bool recolor_##K##_##V(node(K,V) *node) {	\
	node(K,V) *p = parent(node);	\
	node(K,V) *u = uncle(node);	\
	node(K,V) *g = grandparent(node):	\
	p->color = BLACK;	\
	u->color = BLACK;	\
	g->color = RED;	\
}	\
	\
static inline void rotate_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *p = parent(node);	\
	node(K,V) *g = grandparent(node);	\
	if (node == p->right && p == g->left)
		rotate_right_##K##_##V(tree, p);	\
	else if (node == p->left && p == g->right)	\
		rotate_left_##K##_##V(tree, p);	\
	/* finish this */	\
}	\
	\
static inline void repair_tree_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	node(K,V) *temp = node;	\
	while (true) {	\
		if (temp->parent->color == BLACK)	\
			return;	\
		else if (temp->parent == NULL) {	\
			temp->color = BLACK;	\
			return;	\
		}	\
		else if (uncle(node) != NULL && uncle_color(node) == RED) {	\
			temp = grandparent(temp);	\
			recolor_##K##_##V(node);	\
		}	\
		else if (uncle(node) != NULL && uncle_color(node) == BLACK) {	\
			rotate_##K##_##V(tree, node);	\
			return;	\
		}	\
	}	\
}	\
	\
rbtree_code insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	node(K,V) *temp = basic_insert_##K##_##V(tree, key, value);	\
	if (temp == NULL)	\
		return basic_insert_failed;	\
	repair_tree_##K##_##V(tree, node);	\
	return 0;	\
}	\
	\
void set_rbtree_ptr_##K##_##V(rb_tree(K,V) *tree) {	\
	tree->destroy_rbtree = &destroy_rbtree_##K##_##V;	\
	tree->insert = &insert_##K##_##V;	\
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
	tree->root = NULL;	\
	tree->sentinel = NULL;	\
	set_rbtree_ptr_##K##_##V(tree);	\
	return tree;	\
}	\


#define node(K,V)	node_##K##_##V
#define rb_tree(K,V)	rb_tree_##K##_##V
#define new_rbtree(K,V)	new_rbtree_##K##_##V()
#define new_node(K,V)	new_node_##K##_##V()
#endif
