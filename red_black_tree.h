#ifndef RED_BLACK_TREE
#define RED_BLACK_TREE
//#include <stdio.h>
//#include <stdlib.h>
//#include <stddef.h>
//#include <string.h>
//#include <stdbool.h>
//#include <stdint.h>

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

/* generic_node contains all of the operations and members that are common to all nodes.
 * generic_node will be a member of node_##K##_##V (not as a pointer). By using indirection,
 * it can appear as though the generic_node members and operations belong to node_##K##_##V.
 * It seems that this could be used to implement a kind of inheritance in C. The real advantage
 * to generic_node is that the functions and struct will only need to be defined once. I think
 * that will make a big difference in terms of executable size. My original method will lead to
 * the same functions being defined repeatedly for each node_##K##_##V defined. 
 * The purpose of kvnodesize is to allow for efficient copying of nodes
 */

typedef struct generic_node {
	color_t color;
	struct generic_node *parent;
	struct generic_node *rchild;
	struct generic_node *lchild;
	struct generic_node *(*destroy_gnode)(struct generic_node*);
	bool (*is_sentinel)(struct generic_node*);
	struct generic_node *(*set_sentinels)(struct generic_node*, struct generic_node*);
	struct generic_node *(*sibling)(struct generic_node*);
	struct generic_node *(*uncle)(struct generic_node*);
	struct generic_node *(*grandparent)(struct generic_node*);
	struct generic_node *(*minimum)(struct generic_node*);
	struct generic_node *(*maximum)(struct generic_node*);
	struct generic_node *(*successor)(struct generic_node*);
	struct generic_node *(*predecessor)(struct generic_node*);
	size_t kvnodesize;	// This contains the size of the kv node struct, not the generic node struct
} generic_node;

static inline bool is_sentinel(generic_node *node) {
	bool result = true;
	if (node->lchild != NULL && node->rchild != NULL)
		result = false;
	return result;
}

static inline generic_node *make_sentinel() {
	generic_node *node = (generic_node *) calloc(1, sizeof(generic_node));
	node->color = BLACK;
	/* this is done so that sentinels are able to check if they are sentinels */
	node->is_sentinel = &is_sentinel;
	return node;
}

generic_node *set_sentinels(generic_node *node, generic_node *sentinel) {
	if (sentinel == NULL) {
		sentinel = make_sentinel(sentinel);
		if (sentinel == NULL)
			return NULL;
	}
	node->rchild = sentinel;
	node->lchild = sentinel;
	return sentinel;
}

generic_node *destroy_gnode(generic_node *node) {
	if (node == NULL)
		return NULL;
	/* only a sentinel will have null children. the tree will take care of this */
	else if (node->is_sentinel(node))
		return NULL;
	if (node->rchild != NULL) {
		node->rchild = destroy_gnode(node->rchild);
	}
	
	if (node->lchild != NULL)
		node->lchild = destroy_gnode(node->lchild);

	fprintf(stderr, "Address of node being destroyed %p\n", (void *) node);
	free(node);
	return NULL;
}

generic_node *sibling(generic_node *node) {
	if (node == NULL)
		return NULL;
	generic_node *p = node->parent;
	if (p == NULL)
		return NULL;
	return (node == p->lchild) ? p->rchild : p->lchild;
}

generic_node *uncle(generic_node *node) {
	generic_node *p = node->parent;
	return p->sibling(p);
}

generic_node *grandparent(generic_node *node) {
	generic_node *p = node->parent;
	return (p == NULL) ? NULL : p->parent;
}

generic_node *minimum(generic_node *node) {
	generic_node *temp = node;
	while (!temp->is_sentinel(temp)) {
		if (temp->is_sentinel(temp->lchild))
			break;
		temp = temp->lchild;
	}
	return temp;
}

generic_node *maximum(generic_node *node) {
	generic_node *temp = node;
	while (!node->is_sentinel(temp)) {
		if (temp->is_sentinel(temp->rchild))
			break;
		temp = temp->rchild;
	}
	return temp;
}

generic_node *successor(generic_node *node) {
	generic_node *temp = node;
	generic_node *p = node->parent;
	if (node->is_sentinel(temp))
		return NULL;
	else if (node->is_sentinel(temp->rchild)) {
		while (p != NULL && temp != p->lchild) {
			temp = p;
			p = p->parent;
		}
		return p;
	}
	else {
		return minimum(temp->rchild);
	}
	return NULL;
}

generic_node *predecessor(generic_node *node) {
	generic_node *temp = node;
	generic_node *parent = node->parent;
	if (node->is_sentinel(temp))
		return NULL;
	else if (node->is_sentinel(temp->lchild)) {
		while (parent != NULL && temp != parent->rchild) {
			temp = parent;
			parent = parent->parent;
		}
		return parent;
	}
	else {
		return maximum(temp->lchild);
	}
	return NULL;
}

static inline void set_node_ptr(generic_node *node) {
	node->destroy_gnode = &destroy_gnode;
	node->set_sentinels = &set_sentinels;
	node->is_sentinel = &is_sentinel;
	node->sibling = &sibling;
	node->uncle = &uncle;
	node->grandparent = &grandparent;
	node->minimum = &minimum;
	node->maximum = &maximum;
	node->successor = &successor;
	node->predecessor = &predecessor;
}

#define define_node(K,V)	\
typedef struct node_##K##_##V {	\
	generic_node gen_node;	\
	K key;	\
	V value;	\
} node_##K##_##V;	\
	\
node(K,V) *new_node_##K##_##V() {	\
	node(K,V) *node = NULL;	\
	generic_node *base = NULL;	\
	\
	node = (node(K,V) *) calloc(1, sizeof(node(K,V)));	\
	fprintf(stderr, "Address of newly created node: %p\n", (void *) node);	\
	if (node == NULL) {	\
		return NULL;	\
	}	\
		\
	base = (generic_node *) node;	\
	base->color = RED;	\
	set_node_ptr(base);	\
	base->kvnodesize = sizeof(node(K,V));	\
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
	generic_node *sentinel;	\
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
		if (tree->root != NULL) {	\
		/* up cast in order to treat root like a generic node */	\
			generic_node *node = (generic_node *) tree->root;	\
			node->destroy_gnode(node);	\
		}	\
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
	generic_node *temp = (generic_node *) tree->root;	\
	K nkey;	\
	int result = 0;	\
	while (temp != tree->sentinel) {	\
		node(K,V) *ntemp = (node(K,V) *) temp;	\
		nkey = ntemp->key;	\
		result = compare_bytes(&key, &nkey, sizeof(K));	\
		if (result == 0)	\
			/* ntemp and temp point to the same locations */	\
			return ntemp;	\
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
	V val;	\
	node(K,V) *temp = basic_search_##K##_##V(tree, key);	\
	if (temp != NULL)	\
		val = temp->value;	\
	else	\
		memset(&val, 0, sizeof(V));	\
	return val;	\
}	\
	\
static inline node(K,V) *basic_insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	generic_node *node = (generic_node *) tree->root;	\
	generic_node *temp = (generic_node *) tree->root;	\
	node(K,V) *ntemp = NULL;	\
	K nkey;	\
	int result = 0;	\
	if (node == NULL) {	\
		temp = (generic_node *) new_node(K,V);	\
		if (temp == NULL)	\
			return NULL;	\
		ntemp = (node(K,V) *) temp;	\
		ntemp->key = key;	\
		ntemp->value = value;	\
		tree->sentinel = temp->set_sentinels(temp, tree->sentinel);	\
		tree->root = ntemp;	\
		return tree->root;	\
	}	\
		\
	while (true) {	\
		node = temp;	\
		ntemp = (node(K,V) *) node;	\
		nkey = ntemp->key;	\
		result = compare_bytes(&key, &nkey, sizeof(K));	\
		if (result == 0) {	\
			ntemp->value = value;	\
			return ntemp;	\
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
	temp = (generic_node *) new_node(K,V);	\
	ntemp = (node(K,V) *) temp;	\
		\
	if (temp == NULL)	\
		return NULL;	\
	temp->set_sentinels(temp, tree->sentinel);	\
	ntemp->key = key;	\
	ntemp->value = value;	\
	temp->color = RED;	\
	temp->parent = node;	\
	(result > 0) ? (node->rchild = temp) : (node->lchild = temp);	\
	return ntemp;	\
}	\
	\
static inline color_t uncle_color_##K##_##V(node(K,V) *node) {	\
	generic_node *temp = (generic_node *) node;	\
	generic_node *u = temp->uncle(temp);	\
	return (u->color == RED) ? RED : BLACK;	\
}	\
static inline void recolor_##K##_##V(node(K,V) *node) {	\
	generic_node *temp = (generic_node *) node;	\
	generic_node *p = temp->parent;	\
	generic_node *u = temp->uncle(temp);	\
	generic_node *g = temp->grandparent(temp);	\
	p->color = BLACK;	\
	u->color = BLACK;	\
	g->color = RED;	\
}	\
	\
static inline void rotate_left_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	generic_node *temp = (generic_node *) node;	\
	generic_node *pivot = (generic_node *) temp->rchild;	\
	generic_node *p = (generic_node *) temp->parent;	\
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
	if ((generic_node *) tree->root == temp) {	\
		tree->root = (node(K,V) *) pivot;	\
	}	\
}	\
	\
static inline void rotate_right_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	generic_node *temp = (generic_node *) node;	\
	generic_node *p = (generic_node *) temp->parent;	\
	generic_node *pivot = (generic_node *) temp->lchild;	\
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
	if ((generic_node *) tree->root == temp) {	\
		tree->root = (node(K,V) *) pivot;	\
	}	\
}	\
	\
static inline void rotate_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	generic_node *temp = (generic_node *) node;	\
	generic_node *p = (generic_node *) temp->parent;	\
	generic_node *g = (generic_node *) temp->grandparent(temp);	\
	if (temp == p->rchild && p == g->lchild) {	\
		rotate_left_##K##_##V(tree, (node(K,V) *) p);	\
		temp = temp->lchild;	\
	}	\
	else if (temp == p->lchild && p == g->rchild) {	\
		rotate_right_##K##_##V(tree, (node(K,V) *) p);	\
		temp = temp->rchild;	\
	}	\
	p = temp->parent;	\
	g = temp->grandparent(temp);	\
	if (temp == p->lchild) {	\
		rotate_right_##K##_##V(tree, (node(K,V) *) g);	\
	}	\
	else {	\
		rotate_left_##K##_##V(tree, (node(K,V) *) g);	\
	}	\
	p->color = BLACK; 	\
	g->color = RED; 	\
}	\
	\
static inline void repair_tree_insert_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	generic_node *temp = (generic_node *) node;	\
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
		else if (temp->uncle(temp) != NULL && uncle_color_##K##_##V((node(K,V) *) temp) == RED) {	\
			recolor_##K##_##V((node(K,V) *) temp);	\
			temp = temp->grandparent(temp);	\
		}	\
		else if (temp->uncle(temp) != NULL && uncle_color_##K##_##V((node(K,V) *) temp) == BLACK) {	\
			rotate_##K##_##V(tree, (node(K,V) *) temp);	\
			return;	\
		}	\
	}	\
}	\
	\
rbtree_code insert_##K##_##V(rb_tree(K,V) *tree, K key, V value) {	\
	/* Insert and then perform tree repairs */	\
	fprintf(stderr, "In insert\n");	\
	generic_node *temp = (generic_node *) basic_insert_##K##_##V(tree, key, value);	\
	if (temp == NULL)	\
		return basic_insert_failed;	\
	repair_tree_insert_##K##_##V(tree, (node(K,V) *) temp);	\
	return 0;	\
}	\
	\
/* Switch node and child */	\
static inline void transplant_##K##_##V(rb_tree (K,V) *tree, node(K,V) *node, node(K,V) *child) {	\
	generic_node *ngen = (generic_node *) node;	\
	generic_node *cgen = (generic_node *) child;	\
	cgen->parent = ngen->parent;	\
	(ngen == ngen->parent->lchild) ? (ngen->parent->lchild = cgen) : (ngen->parent->rchild = cgen);	\
}	\
	\
/* basic delete is basic in the sense that it doesn't handle repair	*/	\
static inline node(K,V) *basic_delete_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	static bool successor = true;	\
	generic_node *temp = (generic_node *) node;	\
	node(K,V) *ntemp = NULL;	\
	node(K,V) *nreplace = NULL;	\
	/* case where both children are not sentinels */	\
	/* tree does not need its root updated since the node is copied over */	\
	if (!temp->rchild->is_sentinel(temp->rchild) && !temp->lchild->is_sentinel(temp->lchild)) {	\
		generic_node *replace = NULL;	\
		ntemp = (node(K,V) *) temp;	\
		/* alternating between successor and predecessor could help balance tree */	\
		if (successor) {	\
			replace = temp->successor(temp);	\
			successor = false;	\
		}	\
		else {	\
			replace = temp->predecessor(temp);	\
			successor = true;	\
		}	\
		nreplace = (node(K,V) *) replace;	\
		ntemp->key = nreplace->key;	\
		ntemp->value = nreplace->value;	\
		/* If either child of replace is a non sentinel, then assign to the parent of successor */	\
		if (!replace->lchild->is_sentinel(replace->lchild))	\
			transplant_##K##_##V(tree, (node(K,V) *) replace, (node(K,V) *) replace->lchild);	\
			\
		else if (!replace->rchild->is_sentinel(replace->rchild))	\
			transplant_##K##_##V(tree, (node(K,V) *) replace, (node(K,V) *) replace->rchild);	\
		/* Both of successor's children are sentinels means that successor's */	\
		/* spot can be sentinel in the parent of successor */	\
		else if (replace->rchild->is_sentinel(replace->rchild) &&	\
				replace->lchild->is_sentinel(replace->lchild)) {	\
			(replace == replace->parent->lchild)	\
								? (replace->parent->lchild = tree->sentinel)	\
								: (replace->parent->rchild = tree->sentinel);	\
		}	\
		fprintf(stderr, "Neither children are sentinel. Address of node being destroyed: %p\n", nreplace);	\
		free(nreplace);	\
		return NULL;	\
	}	\
	/* if both children are sentinels, then we can go ahead and just delete the node */	\
	else if (temp->rchild->is_sentinel(temp->rchild) && temp->lchild->is_sentinel(temp->lchild)) {	\
		/* If the node in question is root, then set tree root to null */	\
		if (ntemp == tree->root)	\
			tree->root = NULL;	\
		/* The parent of the node being deleted replaces the node with a sentinel */	\
		else	\
			(temp == temp->parent->lchild) ? (temp->parent->lchild = tree->sentinel) : (temp->parent->rchild = tree->sentinel);	\
		fprintf(stderr, "Both children are sentinel. Address of node being destroyed %p\n", node);	\
		free(node);	\
		return NULL;	\
	}	\
	/* in the final case, one child is non-leaf and the other is a sentinel */	\
	else {	\
		generic_node *child = (temp->rchild == tree->sentinel) ? temp->lchild : temp->rchild;	\
		transplant_##K##_##V(tree, (node(K,V) *) temp, (node(K,V) *) child);	\
		/* The case where temp is red is trivial. This only happens when both children are sentinels */	\
		/* This was already handled */	\
		if (temp->color == BLACK) {	\
			if (child->color == BLACK)	\
				return (node(K,V) *) child;	\
			child->color = BLACK;	\
		}	\
		fprintf(stderr, "One child is a sentinel. Address of node being destroyed %p\n", node);	\
		free(node);	\
		return NULL;	\
	}	\
	/* The function should never get to this point */	\
	fprintf(stderr, "Entered impossible case\n");	\
	return NULL;	\
}	\
	\
/* Altering node/child does not actually matter since the caller never uses it */	\
/* after repair tree returns. Since I have a temp node equal to the node parameter */	\
/* for every other function, I'm going to do that here as well in order to avoid confusion */	\
/* This code is based on code from Wikipedia. The algorithm has been altered in multiple ways, however. */	\
void repair_tree_delete_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node) {	\
	generic_node *temp = (generic_node *) node;	\
	/* Using while loop avoid need to use recursion */	\
	while (true) {	\
		/* inverse of case 1 */	\
		if (temp->parent == NULL) {	\
			return;	\
		}	\
		generic_node *sibling = temp->sibling(temp);	\
		/* case two moves to case three */	\
		if (sibling->color == RED) {	\
			temp->parent->color = RED;	\
			sibling->color = BLACK;	\
			/* rotate right or left depending on which child temp is */	\
			(temp == temp->parent->rchild) ? rotate_right_##K##_##V(tree, (node(K,V) *) temp->parent) :	\
					rotate_left_##K##_##V(tree, (node(K,V) *) temp->parent);	\
		}	\
		sibling = temp->sibling(temp);	\
		/* case three goes back to case one */	\
		if (temp->parent->color == BLACK && sibling->color == BLACK && 	\
			sibling->rchild->color == BLACK && sibling->lchild->color == BLACK) {	\
			sibling->color = RED;	\
			temp = temp->parent;	\
			continue;	\
		}	\
		sibling = temp->sibling(temp);	\
		/* case four exits when it is finished */	\
		if (temp->parent->color == RED && sibling->color == BLACK &&	\
			sibling->lchild == BLACK && sibling->rchild == BLACK) {	\
			sibling->color = RED;	\
			temp->parent->color = BLACK;	\
			return;	\
		}	\
		sibling = temp->sibling(temp);	\
		/* case five proceeds to case six */	\
		if (sibling->color == BLACK) {	\
			if (temp == temp->parent->lchild && sibling->rchild == BLACK &&	\
				sibling->lchild->color == RED) {	\
				sibling->color = RED;	\
				sibling->lchild->color = BLACK;	\
				rotate_right_##K##_##V(tree, (node(K,V) *) sibling);	\
			}	\
			else if (temp == temp->parent->rchild && sibling->lchild->color == BLACK &&	\
					sibling->rchild->color == RED) {	\
				sibling->color = RED;	\
				sibling->rchild->color = BLACK;	\
				rotate_left_##K##_##V(tree, (node(K,V) *) sibling);	\
			}	\
		}	\
		sibling = temp->sibling(temp);	\
		sibling->color = temp->parent->color;	\
		temp->parent->color = BLACK;	\
		\
		/* case six exits on finish */	\
		if (temp == temp->parent->lchild) {	\
			sibling->rchild->color = BLACK;	\
			rotate_left_##K##_##V(tree, (node(K,V) *) temp->parent);	\
		}	\
		else {	\
			sibling->lchild->color = BLACK;	\
			rotate_right_##K##_##V(tree, (node(K,V) *) temp->parent);	\
		}	\
		return;	\
	}	\
}	\
	\
rbtree_code delete_##K##_##V(rb_tree(K,V) *tree, K key) {	\
	fprintf(stderr, "In delete\n");	\
	node(K,V) *temp = basic_search_##K##_##V(tree, key);	\
	node(K,V) *child = NULL;	\
	if (temp == NULL)	\
		return key_not_found;	\
	\
	child = basic_delete_##K##_##V(tree, temp);		\
	/* NULL indicates that no tree repair is necessary */	\
	if (child == NULL)	\
		return 0;	\
	/* basic delete does not delete the node in question when a child */	\
	/* is returned. This is necessary for tree repair. */	\
	/* For this reason, temp is freed after repair tree returns */	\
	else {	\
		repair_tree_delete_##K##_##V(tree, child);	\
		/* if the deleted node is root, then child replaces node as root */	\
		if (temp == tree->root)	\
			tree->root = child;	\
		fprintf(stderr, "Address of node being deleted %p\n", (void *) temp);	\
		free(temp);	\
	}	\
	\
	return 0;	\
}	\
void inorder_traverse_##K##_##V(rb_tree(K,V) *tree, node(K,V) *node)	{	\
	generic_node *temp = (generic_node *) node;	\
	/* No need to print sentinel */	\
	if (temp->is_sentinel(temp))	\
		return;	\
	inorder_traverse_##K##_##V(tree, (node(K,V) *) temp->lchild);	\
	if (temp->parent != NULL) {	\
		node(K,V) *parent = (node(K,V) *) temp->parent;	\
		fprintf(stderr, "Node is the child of %d\n", parent->key);	\
	}	\
	else	\
		fprintf(stderr, "Node is root\n");	\
		\
	fprintf(stderr, "Key: %d, Value: %c\n", node->key, node->value);	\
	fprintf(stderr, "Color: "); PRINT_COLOR(temp); fprintf(stderr, "\n");	\
	inorder_traverse_##K##_##V(tree, (node(K,V) *) temp->rchild);	\
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
